/**
 * @file mode_manager.h
 * @brief Top-level operating-mode state machine for the gateway.
 *
 * Owns the FR_005 state machine; consumes button, reed and CC1101 events and
 * drives the LED and buzzer outputs together with the persistent sensor list.
 */
#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <stdint.h>

/**
 * @brief Gateway operating modes (FR_005).
 */
typedef enum {
  MODE_WAITING_FOR_CONFIG = 0,    /**< No sensors paired (FR_007, FR_010). */
  MODE_MONITORING,                /**< >= 1 sensor paired (FR_008, FR_011, FR_012). */
  MODE_PAIRING_UNPAIRING,         /**< Pair/unpair window open (FR_015, FR_016). */
  MODE_WAITING_FOR_FACTORY_RESET, /**< Waiting for factory reset (FR_024, FR_025). */
} mode_t;

/**
 * @brief Initialise the mode state machine.
 *
 * Picks the initial mode based on the paired-sensor count restored from NVM
 * (FR_006). Must be called after @c SensorManager_LoadMirror() and
 * @c board_init().
 */
void mode_manager_init(void);

/**
 * @brief Main-loop tick.
 *
 * Drives mode-dependent indicators and timeouts (FR_004, FR_007, FR_008,
 * FR_010, FR_015, FR_022, FR_023).
 *
 * @param now Current millisecond tick.
 */
void mode_manager_handle(uint16_t now);

/**
 * @brief Get the current operating mode.
 *
 * @return The active @ref mode_t value.
 */
mode_t mode_manager_get(void);

/**
 * @brief Notify the mode manager of a short button press (FR_014, FR_017).
 */
void mode_manager_on_short_press(void);

/**
 * @brief Notify the mode manager that a long press has been detected
 *        (>= 5 s, FR_024).
 */
void mode_manager_on_long_press_detected(void);

/**
 * @brief Notify the mode manager that a long press has been released
 *        (FR_024, FR_025).
 */
void mode_manager_on_long_press_released(void);

/**
 * @brief Process an inbound RF packet from a remote sensor.
 *
 * Routes to pair/unpair logic when in @ref MODE_PAIRING_UNPAIRING
 * (FR_018-FR_021), otherwise to the alert pipeline (FR_012).
 *
 * @param sensor_id 32-bit chip ID of the remote sensor.
 * @param status    Status byte as defined in `sensor_manager.h`.
 */
void mode_manager_on_sensor_packet(uint32_t sensor_id, uint8_t status);

/**
 * @brief Perform a factory reset (FR_009, FR_024, FR_026).
 *
 * Clears NVM and returns to @ref MODE_WAITING_FOR_CONFIG.
 */
void mode_manager_factory_reset(void);

/**
 * @brief Notify the mode manager that the gateway's local door was opened.
 *
 * @return The new internal door-open flag (always 1).
 */
uint8_t mode_manager_on_door_opened(void);

/**
 * @brief Notify the mode manager that the gateway's local door was closed.
 *
 * @return The new internal door-open flag (always 0).
 */
uint8_t mode_manager_on_door_closed(void);

#endif /* MODE_MANAGER_H */
