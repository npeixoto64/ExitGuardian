#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"

/* ---------- LED aliases ---------- */
#define LED_R_PORT        GPIOA
#define LED_R_PIN         GPIO_Pin_4
#define LED_Y_PORT        GPIOA
#define LED_Y_PIN         GPIO_Pin_3
#define LED_B_PORT        GPIOA
#define LED_B_PIN         GPIO_Pin_2

/* ---------- Input aliases ---------- */
#define IRQ_CC1101_PORT   GPIOD
#define IRQ_CC1101_PIN    GPIO_Pin_0
#define IRQ_CC1101_EXTI   EXTI_Pin_0

#define PUSH_BTN_PORT     GPIOD
#define PUSH_BTN_PIN      GPIO_Pin_4
#define PUSH_BTN_EXTI     EXTI_Pin_4

#define REED_DOOR_PORT    GPIOD
#define REED_DOOR_PIN     GPIO_Pin_5
#define REED_DOOR_EXTI    EXTI_Pin_5

/* ---------- Public API ---------- */

/* Full board bring-up: clocks, GPIOs, ADC, I2C, SPI, USART, TIM1, EXTI. */
void board_init(void);

/* Buzzer (TIM1 differential PWM on PD7/PD2). */
void board_buzzer(uint8_t on);

/* Millisecond tick (TIM4 1 kHz). Wraps every ~65.5 s. */
uint16_t board_get_tick_ms(void);

/* TIM4 update ISR hook: clear flag and bump tick. Call from main.c ISR. */
void board_systick_irq(void);

/* Low-power: HALT until EXTI wakes the MCU; restores active peripherals. */
void board_enter_deep_sleep(void);

#endif /* BOARD_H */
