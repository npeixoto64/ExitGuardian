/**
 * @file led.c
 * @brief Status LED state machines for the red, yellow and blue LEDs.
 */
#include "led.h"
#include "board.h"
#include "stm8l15x_gpio.h"

#define LEDB_PERIOD_MS   2000U  /* LED_B blink period (ms) */
#define LEDB_ON_MS        100U  /* LED_B on duration (ms) */

#define LEDR_PERIOD_MS   1000U  /* LED_R blink period (ms) */
#define LEDR_ON_MS        500U  /* LED_R on duration (ms) */

#define LEDY_PERIOD_MS   1000U  /* LED_Y blink period (ms) */
#define LEDY_ON_MS        500U  /* LED_Y on duration (ms) */

/**
 * @brief Per-LED runtime state used by @ref led_mode_handle.
 */
typedef struct {
  uint16_t last_tick;  /**< Last tick at which the LED entered the ON phase. */
  uint8_t  on;         /**< Current physical state of the LED (1 = on). */
  uint8_t  prev_mode;  /**< Last @ref led_mode_t handled, used to detect transitions. */
} led_state_t;

/**
 * @brief Generic LED display handler used by the red and yellow channels.
 *
 * @param port      GPIO port driving the LED.
 * @param pin       GPIO pin driving the LED.
 * @param now       Current millisecond tick.
 * @param mode      Desired LED display mode.
 * @param period_ms Flash period in milliseconds (used in FLASH mode).
 * @param on_ms     On-time in milliseconds (used in FLASH mode).
 * @param st        Per-LED state object.
 */
static void led_mode_handle(GPIO_TypeDef *port, uint8_t pin,
                            uint16_t now, led_mode_t mode,
                            uint16_t period_ms, uint16_t on_ms,
                            led_state_t *st)
{
  /* Act on mode transitions only. */
  if (mode != st->prev_mode) {
    st->prev_mode = mode;
    switch (mode) {
    case LED_MODE_OFF:
      GPIO_ResetBits(port, pin);
      st->on = 0;
      return;
    case LED_MODE_ON:
      GPIO_SetBits(port, pin);
      st->on = 1;
      return;
    case LED_MODE_FLASH:
    case LED_MODE_HEARTBEAT:
      /* Entering FLASH or HEARTBEAT: start a fresh period in the ON phase. */
      GPIO_SetBits(port, pin);
      st->on = 1;
      st->last_tick = now;
      break;
    }
  }

  /* Steady OFF/ON: nothing to do per tick. */
  if (mode != LED_MODE_FLASH && mode != LED_MODE_HEARTBEAT) {
    return;
  }

  if (mode == LED_MODE_HEARTBEAT) {
    /* HEARTBEAT overrides to a fixed 100 ms on-time every 2 s. */
    period_ms = LEDB_PERIOD_MS;
    on_ms = LEDB_ON_MS;
  }

  if (!st->on && (uint16_t)(now - st->last_tick) >= period_ms) {
    GPIO_SetBits(port, pin); // Turn ON
    st->on = 1;
    st->last_tick = now;
  }
  if (st->on && (uint16_t)(now - st->last_tick) >= on_ms) {
    GPIO_ResetBits(port, pin); // Turn OFF
    st->on = 0;
    // Do not update last_tick here
  }
}

/**
 * @brief Drive the blue heartbeat LED (100 ms pulse every 2 s).
 *
 * @param now Current millisecond tick.
 */
// Function to handle periodic LED_B ON every 2s for 100ms
void led_b_handle(uint16_t now)
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

/**
 * @brief Drive the red LED in the requested mode.
 *
 * @param now  Current millisecond tick.
 * @param mode Desired LED display mode.
 */
// Function to handle LED_R: off, on, or flashing every 1s for 500ms
void led_r_handle(uint16_t now, led_mode_t mode)
{
  static led_state_t st = {0, 0, 0};
  led_mode_handle(LED_R_PORT, LED_R_PIN, now, mode,
                  LEDR_PERIOD_MS, LEDR_ON_MS, &st);
}

/**
 * @brief Drive the yellow LED in the requested mode.
 *
 * @param now  Current millisecond tick.
 * @param mode Desired LED display mode.
 */
// Function to handle LED_Y: off, on, or flashing every 1s for 500ms
void led_y_handle(uint16_t now, led_mode_t mode)
{
  static led_state_t st = {0, 0, 0};
  led_mode_handle(LED_Y_PORT, LED_Y_PIN, now, mode,
                  LEDY_PERIOD_MS, LEDY_ON_MS, &st);
}