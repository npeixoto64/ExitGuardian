/**
 * @file reed.h
 * @brief Reed switch (door sensor) driver with edge debouncing.
 */
#ifndef REED_H
#define REED_H

#include <stdint.h>

/**
 * @brief Reed switch events produced by the debounce state machine.
 */
typedef enum {
  REED_EVT_NONE = 0,  /**< No event pending. */
  REED_EVT_OPENED,    /**< Door transitioned to OPEN. */
  REED_EVT_CLOSED     /**< Door transitioned to CLOSED. */
} reed_event_t;

/**
 * @brief Reed switch EXTI ISR body.
 *
 * Called from `EXTI5_IRQHandler` in `main.c`. Arms the debounce timer on the
 * first edge; subsequent bounces are ignored.
 */
void reed_isr(void);

/**
 * @brief Process debounced reed events.
 *
 * Call once per main-loop iteration to settle the debounce timer and queue a
 * @ref reed_event_t when the new stable level is known.
 */
void reed_handle(void);

/**
 * @brief Take and clear the latest pending reed event.
 *
 * @return The pending event, or @ref REED_EVT_NONE if none is queued.
 */
reed_event_t reed_take_event(void);

#endif /* REED_H */