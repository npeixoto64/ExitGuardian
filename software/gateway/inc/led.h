/**
 * @file led.h
 * @brief Status LED state machines (off / on / flash / heartbeat).
 */
#ifndef LED_H
#define LED_H

#include <stdint.h>

/**
 * @brief LED display modes shared by the red and yellow LEDs.
 */
typedef enum {
  LED_MODE_OFF = 0,    /**< Steady off. */
  LED_MODE_ON,         /**< Steady on. */
  LED_MODE_FLASH,      /**< Periodic flash with per-LED period/on-time. */
  LED_MODE_HEARTBEAT,  /**< Short blip every 2 s (100 ms on). */
} led_mode_t;

/**
 * @brief Drive the blue heartbeat LED.
 *
 * Produces a fixed 100 ms pulse every 2 s.
 *
 * @param now Current millisecond tick.
 */
void led_b_handle(uint16_t now);

/**
 * @brief Drive the red status LED.
 *
 * @param now  Current millisecond tick.
 * @param mode Desired LED display mode.
 */
void led_r_handle(uint16_t now, led_mode_t mode);

/**
 * @brief Drive the yellow status LED.
 *
 * @param now  Current millisecond tick.
 * @param mode Desired LED display mode.
 */
void led_y_handle(uint16_t now, led_mode_t mode);

#endif /* LED_H */
