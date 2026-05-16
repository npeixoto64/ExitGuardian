#include "alert_manager.h"

#include "buzzer.h"
#include "led.h"
#include "sensor_manager.h"

/* FR_013: 2 s buzzer pulse, started within 500 ms of ALERT becoming true. */
#define ALERT_BUZZER_PULSE_MS 2000U

static uint8_t  g_door_open;
static uint8_t  g_alert_active;
static uint8_t  g_enabled;
static uint8_t  g_buzzer_running;
static uint16_t g_buzzer_started_ms;

void alert_manager_init(void)
{
  g_door_open         = 0U;
  g_alert_active      = 0U;
  g_enabled           = 0U;
  g_buzzer_running    = 0U;
  g_buzzer_started_ms = 0U;
}

void alert_manager_set_door_open(uint8_t open)
{
  g_door_open = open ? 1U : 0U;
}

void alert_manager_set_enabled(uint8_t enabled)
{
  g_enabled = enabled ? 1U : 0U;
  if (g_enabled == 0U)
  {
    /* FR_016: alerts suspended -> truncate buzzer + drop alert flag. */
    g_alert_active   = 0U;
    g_buzzer_running = 0U;
  }
}

void alert_manager_evaluate(void)
{
  uint8_t prev = g_alert_active;
  uint8_t cond;

  if (g_enabled == 0U)
  {
    g_alert_active = 0U;
    return;
  }

  /* FR_012: ALERT <=> (main door OPEN) AND
   *                   (>= 1 paired sensor with last-known state OPEN). */
  cond = (g_door_open != 0U) && (SensorManager_AnyValidReedSwitchSet() != 0U);
  g_alert_active = cond ? 1U : 0U;

  /* FR_013: on rising edge start the buzzer pulse. */
  if ((prev == 0U) && (g_alert_active != 0U))
  {
    g_buzzer_running    = 1U;
    g_buzzer_started_ms = 0U;  /* set on first handle() tick below */
  }

  /* FR_013: on falling edge truncate the buzzer pulse. */
  if ((prev != 0U) && (g_alert_active == 0U))
  {
    g_buzzer_running = 0U;
  }
}

uint8_t alert_manager_is_active(void)
{
  return g_alert_active;
}

void alert_manager_handle(uint16_t now)
{
  /* Red LED: blinking while alert is active, otherwise OFF (FR_008/FR_013). */
  led_r_handle(now, g_alert_active ? LED_MODE_FLASH : LED_MODE_OFF);

  /* Buzzer: single 2 s pulse on alert rising edge (FR_013). */
  if (g_buzzer_running != 0U)
  {
    if (g_buzzer_started_ms == 0U)
    {
      g_buzzer_started_ms = now;
    }

    if ((uint16_t)(now - g_buzzer_started_ms) >= ALERT_BUZZER_PULSE_MS)
    {
      g_buzzer_running = 0U;
    }
  }

  buzzer_handle(now, g_buzzer_running ? BUZZER_MODE_LONG : BUZZER_MODE_OFF);
}
