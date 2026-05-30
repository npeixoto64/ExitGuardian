/**
 * @file board.h
 * @brief Board support package for the STM8L151 gateway.
 *
 * Defines pin aliases for LEDs, buzzer, push-button, reed switch and CC1101
 * interrupt line, and declares the board-level initialisation and power
 * management API.
 */
#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"

/** @name Status LED pin aliases
 *  Red / Yellow / Blue LEDs driven on GPIOA.
 *  @{ */
#define LED_R_PORT        GPIOA              /**< Red LED GPIO port. */
#define LED_R_PIN         GPIO_Pin_4         /**< Red LED GPIO pin. */
#define LED_Y_PORT        GPIOA              /**< Yellow LED GPIO port. */
#define LED_Y_PIN         GPIO_Pin_3         /**< Yellow LED GPIO pin. */
#define LED_B_PORT        GPIOA              /**< Blue LED GPIO port. */
#define LED_B_PIN         GPIO_Pin_2         /**< Blue LED GPIO pin. */
/** @} */

/** @name Output pin aliases
 *  @{ */
#define BUZZER_EN_PORT    GPIOC              /**< Buzzer enable GPIO port. */
#define BUZZER_EN_PIN     GPIO_Pin_4         /**< Buzzer enable GPIO pin. */
/** @} */

/** @name Input pin aliases (EXTI sources)
 *  @{ */
#define IRQ_CC1101_PORT   GPIOD              /**< CC1101 GDO0 IRQ GPIO port. */
#define IRQ_CC1101_PIN    GPIO_Pin_0         /**< CC1101 GDO0 IRQ GPIO pin. */
#define IRQ_CC1101_EXTI   EXTI_Pin_0         /**< CC1101 GDO0 EXTI line. */

#define PUSH_BTN_PORT     GPIOD              /**< Push-button GPIO port. */
#define PUSH_BTN_PIN      GPIO_Pin_4         /**< Push-button GPIO pin. */
#define PUSH_BTN_EXTI     EXTI_Pin_4         /**< Push-button EXTI line. */

#define REED_DOOR_PORT    GPIOD              /**< Reed switch GPIO port. */
#define REED_DOOR_PIN     GPIO_Pin_5         /**< Reed switch GPIO pin. */
#define REED_DOOR_EXTI    EXTI_Pin_5         /**< Reed switch EXTI line. */
/** @} */

/* ---------- Public API ---------- */

/**
 * @brief Full board bring-up.
 *
 * Configures clocks, GPIOs, ADC, I2C, SPI, USART, TIM1 (buzzer PWM),
 * TIM4 (1 kHz systick), EXTI lines and the independent watchdog, then
 * enables global interrupts and brings the CC1101 up in RX mode.
 */
void board_init(void);

/**
 * @brief Enable or disable the buzzer.
 *
 * Drives the TIM1 differential PWM output on PD7/PD2.
 *
 * @param on Non-zero to start the tone, zero to stop it.
 */
void board_buzzer(uint8_t on);

/**
 * @brief Get the current millisecond tick.
 *
 * Driven by TIM4 at 1 kHz. The 16-bit counter wraps every ~65.5 s, so always
 * compute elapsed intervals using unsigned subtraction modulo 2^16.
 *
 * @return Current tick value in milliseconds.
 */
uint16_t board_get_tick_ms(void);

/**
 * @brief TIM4 update IRQ hook.
 *
 * Clears the TIM4 update flag and increments the millisecond tick. Must be
 * called from the TIM4 update interrupt handler in `main.c`.
 */
void board_systick_irq(void);

/**
 * @brief Refresh (kick) the independent watchdog.
 *
 * Call periodically from the main loop to prevent an IWDG-induced reset.
 */
void board_iwdg_refresh(void);

/**
 * @brief Enter low-power HALT mode.
 *
 * Disables ADC/I2C/SPI/USART before issuing `halt()`. The MCU is woken by an
 * EXTI event (button, reed, CC1101). Peripheral clocks and modules are
 * re-enabled before returning to the caller.
 */
void board_enter_deep_sleep(void);

#endif /* BOARD_H */
