/**
 * @file sensor_manager.h
 * @brief Persistent list of paired sensors backed by a RAM mirror and FRAM.
 *
 * Defines the on-the-wire status byte layout, the record format used in the
 * FRAM-backed mirror, and the public API used by `mode_manager` to pair,
 * unpair and update sensors.
 */
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>

#define SENSOR_MANAGER_RECORD_SIZE 8       /**< Size in bytes of one FRAM record. */
#define SENSOR_MANAGER_MAX_SENSORS 100U    /**< Maximum number of sensors stored. */

/*
 * Status layout (MSB -> LSB):
 * [BAT SoC(4 bits), 0, 0, pairing(2 bits), reed-switch]
 * Validity is stored separately in SensorManagerEntry.valid and in
 * the reserved byte (record[5]) of the FeRAM record.
 *
 * Pairing field values (bits 2:1):
 *   00 - ignore
 *   01 - pairing request
 *   10 - unpairing request
 *   11 - ignore
 */

/** @name Status byte bit positions
 *  @{ */
#define SENSOR_STATUS_REED_SWITCH_BIT      0U  /**< Bit position of the reed-switch state. */
#define SENSOR_STATUS_PAIRING_SHIFT        1U  /**< Shift of the 2-bit pairing field. */
#define SENSOR_STATUS_BAT_SOC_SHIFT        4U  /**< Shift of the 4-bit battery SoC field. */
/** @} */

/** @name Pairing field values
 *  @{ */
#define SENSOR_STATUS_PAIRING_REQUEST      2U  /**< Pairing requested by the sensor. */
#define SENSOR_STATUS_UNPAIRING_REQUEST    3U  /**< Unpairing requested by the sensor. */
/** @} */

/** @name Status byte field masks
 *  @{ */
#define SENSOR_STATUS_REED_SWITCH_MASK     (1U << SENSOR_STATUS_REED_SWITCH_BIT)
#define SENSOR_STATUS_PAIRING_MASK         (0x03U << SENSOR_STATUS_PAIRING_SHIFT)
#define SENSOR_STATUS_BAT_SOC_MASK         (0x0FU << SENSOR_STATUS_BAT_SOC_SHIFT)
/** @} */

/** @name Result codes returned by SensorManager update/pair operations.
 *  @{ */
#define SENSOR_IGNORED                     0U  /**< Operation had no effect. */
#define SENSOR_UPDATED                     1U  /**< Existing valid entry was updated. */
#define SENSOR_PAIRED                      2U  /**< New entry was paired/validated. */
#define SENSOR_UNPAIRED                    3U  /**< Existing entry was invalidated. */
/** @} */

/* Status byte format:
*   Bits 7-4: Battery capacity (high nibble)
*   Bits 2-1: Button action (0(none) = reed update, 1(single press) = force update, 2(double press) = pair, 3(long press) = unpair)
*   Bit 0:    Reed switch state
*/

/** @brief Extract the 4-bit battery SoC from a status byte. */
#define SENSOR_STATUS_GET_BAT_SOC(status) \
    (uint8_t)(((uint8_t)(status) & SENSOR_STATUS_BAT_SOC_MASK) >> SENSOR_STATUS_BAT_SOC_SHIFT)
/** @brief Extract the 2-bit pairing field from a status byte. */
#define SENSOR_STATUS_GET_PAIRING(status) \
    (uint8_t)(((uint8_t)(status) & SENSOR_STATUS_PAIRING_MASK) >> SENSOR_STATUS_PAIRING_SHIFT)
/** @brief Non-zero if the status byte encodes a pairing request. */
#define SENSOR_STATUS_IS_PAIRING(status) \
    (uint8_t)((SENSOR_STATUS_GET_PAIRING(status) == SENSOR_STATUS_PAIRING_REQUEST) ? 1U : 0U)
/** @brief Non-zero if the status byte encodes an unpairing request. */
#define SENSOR_STATUS_IS_UNPAIRING(status) \
    (uint8_t)((SENSOR_STATUS_GET_PAIRING(status) == SENSOR_STATUS_UNPAIRING_REQUEST) ? 1U : 0U)
/** @brief Non-zero if the reed-switch bit is set in @p status. */
#define SENSOR_STATUS_IS_REED_SWITCH(status) \
    (uint8_t)((((uint8_t)(status) & SENSOR_STATUS_REED_SWITCH_MASK) != 0U) ? 1U : 0U)
/** @brief Non-zero if the battery SoC indicates a low-battery condition (<= 5/15). */
#define SENSOR_STATUS_IS_LOW_BATTERY(status) \
    (uint8_t)((SENSOR_STATUS_GET_BAT_SOC(status) <= 5U) ? 1U : 0U)

/**
 * @brief In-RAM mirror entry for one sensor record.
 */
typedef struct {
    uint32_t id;     /**< 32-bit sensor chip ID. */
    uint8_t status;  /**< Last known status byte (pairing bits stripped). */
    uint8_t valid;   /**< Non-zero when the entry holds a paired sensor. */
} SensorManagerEntry;

/**
 * @brief Load all sensor records from FRAM into the internal RAM mirror.
 */
void SensorManager_LoadMirror(void);

/**
 * @brief Reset the FRAM metadata header and clear the RAM mirror.
 *
 * Marks all stored entries as invalid and writes a fresh empty header.
 */
void SensorManager_ResetFeramHeaderAndMirror(void);

/**
 * @brief Update an existing sensor's status.
 *
 * Behaviour:
 *  - If @p id matches a valid entry and @p status differs, updates RAM and
 *    persists to FRAM.
 *  - If @p id is unknown or the matching entry is invalid, ignores the
 *    update.
 *
 * @param id     32-bit sensor chip ID.
 * @param status Status byte (pairing bits are stripped before storage).
 *
 * @return @ref SENSOR_UPDATED on a real update, @ref SENSOR_IGNORED otherwise.
 */
uint8_t SensorManager_UpdateSensorStatus(uint32_t id, uint8_t status);

/**
 * @brief Pair or unpair a sensor based on the status byte.
 *
 *  - Pairing request on a new ID: appends a valid entry when there is room.
 *  - Pairing request on an existing invalid ID: revalidates the entry.
 *  - Unpairing request on a valid entry: invalidates it.
 *  - All other combinations are ignored.
 *
 * @param id     32-bit sensor chip ID.
 * @param status Status byte carrying the pairing/unpairing request.
 *
 * @return @ref SENSOR_PAIRED, @ref SENSOR_UNPAIRED or @ref SENSOR_IGNORED.
 */
uint8_t SensorManager_PairUnpairSensor(uint32_t id, uint8_t status);

/**
 * @brief Test whether any paired sensor currently reports its window open.
 *
 * Reads the cached flag last updated by
 * @ref SensorManager_AnyValidReedSwitchSet.
 *
 * @return 1 if at least one valid sensor has its reed-switch bit set, 0 otherwise.
 */
uint8_t SensorManager_IsAnyWindowOpen(void);

/**
 * @brief Test whether any paired sensor currently reports a low battery.
 *
 * Reads the cached flag last updated by @ref SensorManager_AnyLowBatterySet.
 *
 * @return 1 if at least one valid sensor reports low battery, 0 otherwise.
 */
uint8_t SensorManager_IsAnySensorWithLowBattery(void);

/**
 * @brief Count the number of currently paired (valid) sensors.
 *
 * @return Number of valid entries in the RAM mirror.
 */
uint8_t SensorManager_PairedCount(void);

/**
 * @brief Recompute the cached "any window open" flag from the RAM mirror.
 */
void SensorManager_AnyValidReedSwitchSet(void);

/**
 * @brief Recompute the cached "any low battery" flag from the RAM mirror.
 */
void SensorManager_AnyLowBatterySet(void);

#endif
