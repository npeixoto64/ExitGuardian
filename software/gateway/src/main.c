#include <stdint.h>
#include <stdio.h>
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"
#include "board.h"
#include "cc1101.h"
#include "feram.h"
#include "log.h"

static volatile uint8_t  g_irq_cc1101_flag = 0;
static volatile uint8_t  g_reed_door_flag  = 0;

/* Push button state: edges captured in EXTI ISR, processed in main loop. */
#define BTN_DEBOUNCE_MS         20U     /* ignore edges shorter than this */
#define BTN_SHORT_PRESS_MAX_MS  2000U   /* release < 2 s -> short press  */
#define BTN_LONG_PRESS_MIN_MS   5000U   /* release >= 5 s -> long press  */

#define LEDB_PERIOD_MS   2000U  /* LED_B blink period */
#define LEDB_ON_MS        100U  /* LED_B on duration  */

static volatile uint8_t  g_btn_down_evt         = 0;
static volatile uint8_t  g_btn_up_evt           = 0;
static volatile uint16_t g_btn_debounce_ms      = 0;
static volatile uint8_t  btn_handle_debounce    = 0;

/* CC1101 GDO0 falling-edge interrupt (IRQ vector 8, PD0).
 * Set when the CC1101 asserts its interrupt line (active-low),
 * signalling that a packet has been received. Processed in main loop. */
INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
  if (GPIO_ReadInputDataBit(IRQ_CC1101_PORT, IRQ_CC1101_PIN) == RESET)
  {
    g_irq_cc1101_flag = 1;
  }
  EXTI_ClearITPendingBit(EXTI_IT_Pin0);
}

/* Push-button both-edges interrupt (IRQ vector 12, PD4).
 * Debounce is performed in the ISR: any edge closer than BTN_DEBOUNCE_MS to
 * the previous accepted edge is discarded.  Accepted edges update press/release
 * state and timestamps immediately, so events are never lost due to main-loop
 * latency (e.g. while cc1101_recv_msg or send_string is running). */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  if (GPIO_ReadInputDataBit(PUSH_BTN_PORT, PUSH_BTN_PIN) == RESET)
  {
    if (btn_handle_debounce == 0)
    {
      btn_handle_debounce = 1;
      g_btn_debounce_ms = board_get_tick_ms();
    }
  }
  else
  {
    if (btn_handle_debounce == 0)
    {
      btn_handle_debounce = 1;
      g_btn_debounce_ms = board_get_tick_ms();
    }
  }
  
  EXTI_ClearITPendingBit(EXTI_IT_Pin4);
}

/* TIM4 update/overflow interrupt (IRQ vector 25).
 * Fires at 1 kHz; delegates tick increment and flag clear to board driver.
 * 1 ms tick */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  board_systick_irq();
}

/* Reed switch (door sensor) falling-edge interrupt (IRQ vector 13, PD5).
 * Asserted when the door is opened (magnet moves away, pin pulled low).
 * Processed in the main loop via g_reed_door_flag. */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
  if (GPIO_ReadInputDataBit(REED_DOOR_PORT, REED_DOOR_PIN) == RESET)
  {
    g_reed_door_flag = 1;
  }
  EXTI_ClearITPendingBit(EXTI_IT_Pin5);
}

// Function to handle periodic LED_B ON every 2s for 100ms
void periodic_ledb_tick(uint16_t now)
{
  static uint16_t last_tick = 0;
  static uint8_t  state = 0;
  if (!state && (uint16_t)(now - last_tick) >= LEDB_PERIOD_MS) {
    GPIO_SetBits(LED_B_PORT, LED_B_PIN); // Turn ON
    state = 1;
    last_tick = now;
  }
  if (state && (uint16_t)(now - last_tick) >= LEDB_ON_MS) {
    GPIO_ResetBits(LED_B_PORT, LED_B_PIN); // Turn OFF
    state = 0;
    // Do not update last_tick here
  }
}

int main(void)
{
    board_init();
    enableInterrupts();

    cc1101_config_gfsk_433_rx_fixed(5);

    uint8_t status = 0;
    uint32_t chip_id = 0;
    char buffer[64];
    uint8_t buzzer_toggle_flag = 0;


    // State for periodic LED_B
    uint16_t ledb_tick = 0;
    uint8_t  ledb_on = 0;

    while (1) {
      uint16_t now = board_get_tick_ms();

      periodic_ledb_tick(now);

      if (g_irq_cc1101_flag)
      {
        g_irq_cc1101_flag = 0;

        cc1101_recv_msg(&chip_id, &status);

        // buzzer_toggle_flag ^= 1U;
        // board_buzzer(buzzer_toggle_flag);

        sprintf(buffer, "\r\nTransmitter Chip ID: %lx", chip_id);
        send_string(buffer);
        sprintf(buffer, "; status: %lu", (uint32_t)status);
        send_string(buffer);
      }


      if (btn_handle_debounce)
      {
        if ((uint16_t)(board_get_tick_ms() - g_btn_debounce_ms) > BTN_DEBOUNCE_MS)
        {
          btn_handle_debounce = 0;
          if (GPIO_ReadInputDataBit(PUSH_BTN_PORT, PUSH_BTN_PIN) == RESET)
          {
            g_btn_down_evt = 1;
            send_string("\r\ng_btn_down_evt set");
          }
          else
          {
            g_btn_up_evt = 1;
            send_string("\r\ng_btn_up_evt set");
          }
        }
      }
    }
}
