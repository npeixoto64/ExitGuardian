#include "mode_manager.h"

#include "alert_manager.h"
#include "board.h"
#include "buzzer.h"
#include "cc1101.h"
#include "led.h"
#include "log.h"
#include "sensor_manager.h"

#include <stdio.h>

/* 20 s pair/unpair window (FR_015). */
#define MODE_PAIRING_TIMEOUT_MS  20000U

/* 2 s buzzer confirmation pulse on successful pair/unpair (FR_018, FR_020). */
#define MODE_CONFIRM_BUZZER_MS   2000U

static mode_t   g_mode;
static uint16_t g_pairing_started_ms;
static uint16_t g_confirm_started_ms;
static uint8_t  g_confirm_active;

static mode_t mode_from_paired_count(void)
{
  /* FR_006/FR_020: pick mode based on number of paired sensors. */
  return (SensorManager_PairedCount() > 0U)
       ? MODE_MONITORING
       : MODE_WAITING_FOR_CONFIG;
}

static void enter_mode(mode_t next)
{
  g_mode = next;

  switch (next)
  {
    case MODE_WAITING_FOR_CONFIG:
      /* FR_007 red ON, FR_010 yellow OFF, alerts disabled. */
      alert_manager_set_enabled(0U);
      break;

    case MODE_MONITORING:
      /* FR_008 red OFF (driven by alert_manager when not alerting),
       * FR_022 yellow follows low-battery state. */
      alert_manager_set_enabled(1U);
      break;

    case MODE_PAIRING_UNPAIRING:
      /* FR_015 start 20 s window. FR_016 alerts suspended. */
      g_pairing_started_ms = board_get_tick_ms();
      alert_manager_set_enabled(0U);
      break;
  }
}

static void exit_pairing_to_previous(void)
{
  /* FR_015 / FR_017 / FR_019 / FR_021: return to monitoring if any sensor
   * remains paired, otherwise waiting-for-configuration. */
  enter_mode(mode_from_paired_count());
}

void mode_manager_init(void)
{
  g_confirm_active     = 0U;
  g_confirm_started_ms = 0U;
  g_pairing_started_ms = 0U;
  enter_mode(mode_from_paired_count());
}

mode_t mode_manager_get(void)
{
  return g_mode;
}

void mode_manager_on_short_press(void)
{
  switch (g_mode)
  {
    case MODE_WAITING_FOR_CONFIG:
    case MODE_MONITORING:
      /* FR_014: enter pairing/unpairing mode. */
      enter_mode(MODE_PAIRING_UNPAIRING);
      send_string("\r\nMODE: PAIRING_UNPAIRING");
      break;

    case MODE_PAIRING_UNPAIRING:
      /* FR_017: exit pairing/unpairing on short press. */
      exit_pairing_to_previous();
      send_string("\r\nEXIT MODE: PAIRING_UNPAIRINGv -> Button short press");
      break;
  }
}

void mode_manager_on_long_press_detected(void)
{
  enter_mode(MODE_WAITING_FOR_FACTORY_RESET);
}

void mode_manager_on_long_press_released(void)
{
  /* FR_024: factory RESET on >= 5 s press release (FR_025 ignores 2-5 s). */
  send_string("\r\nFactory reset triggered by long press release");
  mode_manager_factory_reset();
}

void mode_manager_on_sensor_packet(uint32_t sensor_id, uint8_t status)
{
  uint8_t updated;

  if (g_mode == MODE_PAIRING_UNPAIRING)
  {
    updated = SensorManager_UpdateSensorStatus(sensor_id, status);

    /* FR_018/FR_020: a successful add/remove triggers the 2 s confirmation
     * buzzer and exits the mode. FR_019/FR_021: ignored requests still exit
     * the mode but without the confirmation pulse. FR_027: no ACK is sent. */
    if (updated != 0U)
    {
      g_confirm_active     = 1U;
      g_confirm_started_ms = board_get_tick_ms();
    }
    exit_pairing_to_previous();
    return;
  }

  // /* MODE_MONITORING: just update last-known state/battery (NFR_008) and
  //  * re-evaluate the alert (FR_012). */
  // (void)SensorManager_UpdateSensorStatus(sensor_id, status);
  // alert_manager_evaluate();
}

void mode_manager_factory_reset(void)
{
  /* FR_009 / FR_026: clear NVM, no sensors attached, return to
   * waiting-for-configuration. */
  SensorManager_ResetFeramHeaderAndMirror();
  alert_manager_set_enabled(0U);
  enter_mode(MODE_WAITING_FOR_CONFIG);
  send_string("\r\nMODE: WAITING_FOR_CONFIG");
}

void mode_manager_handle(uint16_t now)
{
  /* FR_004 blue heartbeat is unconditional. */
  led_b_handle(now);

  switch (g_mode)
  {
    case MODE_WAITING_FOR_CONFIG:
      led_r_handle(now, LED_MODE_ON);     /* FR_007 */
      led_y_handle(now, LED_MODE_OFF);    /* FR_010 */
      buzzer_handle(now, g_confirm_active ? BUZZER_MODE_LONG : BUZZER_MODE_OFF);
      break;

    case MODE_MONITORING:
      led_y_handle(now, LED_MODE_FLASH);  /* FR_022 (driven by sensor_manager battery flag) */
      buzzer_handle(now, g_confirm_active ? BUZZER_MODE_LONG : BUZZER_MODE_OFF);
      alert_manager_handle(now);          /* FR_008/FR_013 own the red LED */
      break;

    case MODE_PAIRING_UNPAIRING:
      led_y_handle(now, LED_MODE_FLASH);  /* FR_015/FR_023 */
      buzzer_handle(now, BUZZER_MODE_BEEP); /* FR_015 */

      /* FR_015: 20 s timeout. */
      if ((uint16_t)(now - g_pairing_started_ms) >= MODE_PAIRING_TIMEOUT_MS)
      {
        exit_pairing_to_previous();
        send_string("\r\nExit mode: PAIRING_UNPAIRING -> timeout");
      }
      break;

    case MODE_WAITING_FOR_FACTORY_RESET:
      // All LEDs flashing and buzzer off (FR_024/FR_025). No timeout (FR_026).
       led_r_handle(now, LED_MODE_FLASH);
       led_y_handle(now, LED_MODE_FLASH);
       buzzer_handle(now, BUZZER_MODE_OFF);
       //alert_manager_handle(now);          /* FR_024/FR_025 own the red LED */
      break;
  }

  /* Auto-clear confirmation buzzer flag after the 2 s pulse. */
  if (g_confirm_active != 0U
      && (uint16_t)(now - g_confirm_started_ms) >= MODE_CONFIRM_BUZZER_MS)
  {
    g_confirm_active = 0U;
  }
}
