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
#define BTN_SHORT_PRESS_MAX_MS  2000U   /* release < 2 s -> short press */
#define BTN_LONG_PRESS_MIN_MS   5000U   /* release >= 5 s -> long press */

static volatile uint8_t  g_btn_pressed     = 0; /* 1 while button is held */
static volatile uint8_t  g_btn_release_evt = 0; /* set on release edge   */
static volatile uint16_t g_btn_press_ms    = 0; /* tick at press edge    */
static volatile uint16_t g_btn_release_ms  = 0; /* tick at release edge  */

INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
{
  if (GPIO_ReadInputDataBit(IRQ_CC1101_PORT, IRQ_CC1101_PIN) == RESET)
  {
    g_irq_cc1101_flag = 1;
  }
  EXTI_ClearITPendingBit(EXTI_IT_Pin0);
}

INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  uint16_t now = board_get_tick_ms();
  if (GPIO_ReadInputDataBit(PUSH_BTN_PORT, PUSH_BTN_PIN) == RESET)
  {
    /* Falling edge: press start. */
    g_btn_pressed  = 1;
    g_btn_press_ms = now;
  }
  else
  {
    /* Rising edge: release. */
    if (g_btn_pressed)
    {
      g_btn_pressed     = 0;
      g_btn_release_ms  = now;
      g_btn_release_evt = 1;
    }
  }
  EXTI_ClearITPendingBit(EXTI_IT_Pin4);
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  board_systick_irq();
}

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
  if (!state && (uint16_t)(now - last_tick) >= 2000) {
    GPIO_SetBits(LED_B_PORT, LED_B_PIN); // Turn ON
    state = 1;
    last_tick = now;
  }
  if (state && (uint16_t)(now - last_tick) >= 100) {
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

        buzzer_toggle_flag ^= 1U;
        board_buzzer(buzzer_toggle_flag);

        sprintf(buffer, "\r\nTransmitter Chip ID: %lx", chip_id);
        send_string(buffer);
        sprintf(buffer, "; status: %lu", (uint32_t)status);
        send_string(buffer);
      }

    //   if (g_btn_release_evt)
    //   {
    //     uint16_t press_ms;
    //     uint16_t release_ms;

    //     __asm__("push cc\n\tsim");
    //     press_ms          = g_btn_press_ms;
    //     release_ms        = g_btn_release_ms;
    //     g_btn_release_evt = 0;
    //     __asm__("pop cc");

    //     uint16_t held_ms = (uint16_t)(release_ms - press_ms);

    //     if (held_ms < BTN_SHORT_PRESS_MAX_MS)
    //     {
    //        /* Short press: release in < 2 s. */
    //        GPIO_ToggleBits(LED_Y_PORT, LED_Y_PIN);
    //        send_string("\r\nBTN: short press");
    //      }
    //      else if (held_ms >= BTN_LONG_PRESS_MIN_MS)
    //      {
    //        /* Long press: release after >= 5 s. */
    //        GPIO_ToggleBits(LED_R_PORT, LED_R_PIN);
    //        send_string("\r\nBTN: long press");
    //      }
    //     /* 2 s..5 s window is intentionally ignored. */
    //   }
    }
}
