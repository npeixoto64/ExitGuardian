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
#include "led.h"
#include "buzzer.h"
#include "mode_manager.h"

static volatile uint8_t  g_irq_cc1101_flag = 0;

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

int main(void)
{
    board_init();
    mode_manager_init();

    uint8_t status = 0;
    uint32_t chip_id = 0;
    char buffer[64];

    while (1) {
      uint16_t now = board_get_tick_ms();

      // Check buttons and reed switch, and handle their events.
      reed_handle();
      button_handle();

      // Get CC1101 packets and pass to mode manager for handling.
      if (g_irq_cc1101_flag)
      {
        g_irq_cc1101_flag = 0;

        cc1101_recv_msg(&chip_id, &status);

        /* Status byte format:
         *   Bits 7-4: Battery capacity (high nibble)
         *   Bits 2-1: Button action (0 = none, 1(single press) = force update, 2(double press) = pair, 3(long press) = unpair)
         *   Bit 0:    Reed switch state
         */
        uint8_t battery     = (status >> 4) & 0x0F;
        uint8_t button_act  = (status >> 1) & 0x03;
        uint8_t reed_state  = status & 0x01;

        sprintf(buffer, "\r\nTransmitter Chip ID: %lx", chip_id);
        send_string(buffer);
        sprintf(buffer, "; battery: %u", battery);
        send_string(buffer);
        sprintf(buffer, "; button: %u", button_act);
        send_string(buffer);
        sprintf(buffer, "; reed: %u", reed_state);
        send_string(buffer);
        send_register_hex("status: ", status);

        mode_manager_on_sensor_packet(chip_id, status);
      }

      // Get button events and pass to mode manager for handling.
      button_event_t button_evt = button_take_event();
      if (button_evt == BUTTON_EVT_SHORT_PRESS)
      {
        send_string("\r\nSHORT_PRESS");
        mode_manager_on_short_press();
      }
      else if (button_evt == BUTTON_EVT_LONG_PRESS_DETECTED)
      {
        send_string("\r\nLONG_PRESS_DETECTED");
        mode_manager_on_long_press_detected();
      }
      else if (button_evt == BUTTON_EVT_LONG_PRESS_RELEASED)
      {
        send_string("\r\nLONG_PRESS_RELEASED");
        mode_manager_on_long_press_released();
      }

      // reed_event_t reed_evt = reed_take_event();
      // if (reed_evt != REED_EVT_NONE)
      // {
      //   sprintf(buffer, "\r\nReed event: %d", reed_evt);
      //   send_string(buffer);
      //   //mode_manager_on_reed_event(reed_evt);
      // }

      mode_manager_handle(board_get_tick_ms());
    }
}
