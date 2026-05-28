/**
 * @file buzzer.h
 * @brief Non-blocking buzzer driver with simple playback modes.
 */
#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

/**
 * @brief Buzzer playback modes consumed by @ref buzzer_handle.
 */
typedef enum {
  BUZZER_MODE_OFF = 0,  /**< Silent. */
  BUZZER_MODE_LONG,     /**< One-shot 2 s tone. */
  BUZZER_MODE_BEEP      /**< 100 ms beep every 1 s. */
} buzzer_mode_t;

/**
 * @brief Run the buzzer state machine.
 *
 * Call every main-loop iteration with the current ms tick and desired mode.
 *  - @ref BUZZER_MODE_OFF: silent.
 *  - @ref BUZZER_MODE_LONG: on entry, tone for 2 s then auto-stops; staying
 *    in LONG does not retrigger. To replay, switch to OFF (or BEEP) and back
 *    to LONG.
 *  - @ref BUZZER_MODE_BEEP: 100 ms tone every 1 s while mode is selected.
 *
 * @param now  Current millisecond tick (see @ref board_get_tick_ms).
 * @param mode Desired playback mode.
 */
void buzzer_handle(uint16_t now, buzzer_mode_t mode);

#endif /* BUZZER_H */
