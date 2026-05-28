/**
 * @file button.h
 * @brief Push-button driver: debounce and short/long-press detection.
 */
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

/**
 * @brief Push-button events produced by the debounce/timing state machine.
 */
typedef enum {
  BUTTON_EVT_NONE = 0,            /**< No event pending. */
  BUTTON_EVT_SHORT_PRESS,         /**< Released within <= 2 s (FR_014/FR_017). */
  BUTTON_EVT_LONG_PRESS_DETECTED, /**< Held for >= 5 s (FR_024). */
  BUTTON_EVT_LONG_PRESS_RELEASED  /**< Released after >= 5 s (FR_024). */
} button_event_t;

/**
 * @brief Push-button EXTI ISR body.
 *
 * Called from `EXTI4_IRQHandler` in `main.c`. Arms the debounce timer on the
 * first edge; subsequent bounces are ignored.
 */
void button_isr(void);

/**
 * @brief Process debounced button events.
 *
 * Call once per main-loop iteration to advance the debounce/timing state
 * machine and enqueue a `button_event_t`.
 */
void button_handle(void);

/**
 * @brief Take and clear the latest pending button event.
 *
 * @return The pending event, or @ref BUTTON_EVT_NONE if none is queued.
 */
button_event_t button_take_event(void);

#endif /* BUTTON_H */
