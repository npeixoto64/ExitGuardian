#include <stdint.h>
#include <stdio.h>
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"
#include "board.h"
#include "cc1101.h"
#include "feram.h"
#include "log.h"
#include "reed.h"
#include "button.h"

static volatile uint8_t  g_irq_cc1101_flag = 0;

#define LEDB_PERIOD_MS   2000U  /* LED_B blink period */
#define LEDB_ON_MS        100U  /* LED_B on duration  */

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
 * ISR kept in main.c so the linker always includes button.rel via button_isr(). */
INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
{
  button_isr();
  EXTI_ClearITPendingBit(EXTI_IT_Pin4);
}

/* Reed switch (door sensor) both-edges interrupt (IRQ vector 13, PD5).
 * ISR kept in main.c so the linker always includes reed.rel via reed_isr(). */
INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
{
  reed_isr();
  EXTI_ClearITPendingBit(EXTI_IT_Pin5);
}

/* TIM4 update/overflow interrupt (IRQ vector 25).
 * Fires at 1 kHz; delegates tick increment and flag clear to board driver.
 * 1 ms tick */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  board_systick_irq();
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

      reed_handle();
      button_handle();
    }
}
