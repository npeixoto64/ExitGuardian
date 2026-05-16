#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include <stdint.h>

/* Reset internal state, no active alert. Call once at boot. */
void alert_manager_init(void);

/* Update last-known main-door state (after debounce, FR_011). */
void alert_manager_set_door_open(uint8_t open);

/* Re-evaluate alert condition (FR_012):
 *   ALERT <=> (main door OPEN) AND (>= 1 paired sensor with last-known OPEN).
 * Call after a debounced door edge or after a sensor packet was applied to
 * sensor_manager. */
void alert_manager_evaluate(void);

/* Suspend/resume the alert pipeline. Used to mute alerts while in
 * pairing/unpairing mode (FR_016). When suspended, evaluate() is a no-op
 * and any active buzzer pulse is truncated. */
void alert_manager_set_enabled(uint8_t enabled);

/* Drive buzzer pulse and red-LED blink (FR_013). Call every main loop. */
void alert_manager_handle(uint16_t now);

/* True while ALERT condition is asserted. */
uint8_t alert_manager_is_active(void);

#endif /* ALERT_MANAGER_H */
