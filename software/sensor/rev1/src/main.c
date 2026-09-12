/**
 * @file main.c
 * @brief Application entry point and top-level interrupt handlers.
 */
#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "device_id.h"
#include "app_config.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"
#include "cc1101.h"
#include "log.h"

// static volatile uint8_t  g_irq_cc1101_flag = 0;

// /* CC1101 GDO0 falling-edge interrupt (IRQ vector 8, PD0).
//  * Set when the CC1101 asserts its interrupt line (active-low),
//  * signalling that a packet has been received. Processed in main loop. */
// INTERRUPT_HANDLER(EXTI0_IRQHandler, 8)
// {
//   if (GPIO_ReadInputDataBit(IRQ_CC1101_PORT, IRQ_CC1101_PIN) == RESET)
//   {
//     g_irq_cc1101_flag = 1;
//   }
//   EXTI_ClearITPendingBit(EXTI_IT_Pin0);
// }

// /* Push-button both-edges interrupt (IRQ vector 12, PD4).
//  * ISR kept in main.c so the linker always includes button.rel via button_isr(). */
// INTERRUPT_HANDLER(EXTI4_IRQHandler, 12)
// {
//   button_isr();
//   EXTI_ClearITPendingBit(EXTI_IT_Pin4);
// }

// /* Reed switch (door sensor) both-edges interrupt (IRQ vector 13, PD5).
//  * ISR kept in main.c so the linker always includes reed.rel via reed_isr(). */
// INTERRUPT_HANDLER(EXTI5_IRQHandler, 13)
// {
//   reed_isr();
//   EXTI_ClearITPendingBit(EXTI_IT_Pin5);
// }

// /* TIM4 update/overflow interrupt (IRQ vector 25).
//  * Fires at 1 kHz; delegates tick increment and flag clear to board driver.
//  * 1 ms tick */
// INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
// {
//   board_systick_irq();
// }

/**
 * @brief Application entry point.
 *
 * Brings up the board and the mode manager, then runs the main super-loop:
 * watchdog refresh, reed/button handling, CC1101 packet dispatch, event
 * forwarding to the mode manager and LED/buzzer tick.
 *
 * @return Never returns.
 */
int main(void)
{
    board_init();
    
    //send_string("\r\n\r\n**** Startup... ****\r\n");

    while (1) {
      uint32_t var = 0;
      while(var < (uint32_t)600000)
      {
        var++;
      }
      send_string("Tick\r\n");
      GPIO_ToggleBits(GPIOB, GPIO_Pin_0);
    //  uint16_t now = board_get_tick_ms();
    }
}
