#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>

#define SENSOR_MANAGER_RECORD_SIZE 8
#define SENSOR_MANAGER_MAX_SENSORS 100U

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
#define SENSOR_STATUS_REED_SWITCH_BIT      0U
#define SENSOR_STATUS_PAIRING_SHIFT        1U
#define SENSOR_STATUS_BAT_SOC_SHIFT        4U

#define SENSOR_STATUS_PAIRING_IGNORE       0U
#define SENSOR_STATUS_PAIRING_REQUEST      1U
#define SENSOR_STATUS_UNPAIRING_REQUEST    2U

#define SENSOR_STATUS_REED_SWITCH_MASK     (1U << SENSOR_STATUS_REED_SWITCH_BIT)
#define SENSOR_STATUS_PAIRING_MASK         (0x03U << SENSOR_STATUS_PAIRING_SHIFT)
#define SENSOR_STATUS_BAT_SOC_MASK         (0x0FU << SENSOR_STATUS_BAT_SOC_SHIFT)

#define SENSOR_STATUS_PACK(bat_soc, pairing, reed_switch) \
    (uint8_t)((((uint8_t)(bat_soc) & 0x0FU) << SENSOR_STATUS_BAT_SOC_SHIFT) \
        | (((uint8_t)(pairing) & 0x03U) << SENSOR_STATUS_PAIRING_SHIFT) \
        | (((reed_switch) ? 1U : 0U) << SENSOR_STATUS_REED_SWITCH_BIT))

#define SENSOR_STATUS_GET_BAT_SOC(status) \
    (uint8_t)(((uint8_t)(status) & SENSOR_STATUS_BAT_SOC_MASK) >> SENSOR_STATUS_BAT_SOC_SHIFT)
#define SENSOR_STATUS_GET_PAIRING(status) \
    (uint8_t)(((uint8_t)(status) & SENSOR_STATUS_PAIRING_MASK) >> SENSOR_STATUS_PAIRING_SHIFT)
#define SENSOR_STATUS_IS_PAIRING(status) \
    (uint8_t)((SENSOR_STATUS_GET_PAIRING(status) == SENSOR_STATUS_PAIRING_REQUEST) ? 1U : 0U)
#define SENSOR_STATUS_IS_UNPAIRING(status) \
    (uint8_t)((SENSOR_STATUS_GET_PAIRING(status) == SENSOR_STATUS_UNPAIRING_REQUEST) ? 1U : 0U)
#define SENSOR_STATUS_IS_REED_SWITCH(status) \
    (uint8_t)((((uint8_t)(status) & SENSOR_STATUS_REED_SWITCH_MASK) != 0U) ? 1U : 0U)

typedef struct {
    uint32_t id;
    uint8_t status;
    uint8_t valid;
} SensorManagerEntry;

/* Loads all sensor records from FeRAM into the internal RAM mirror. */
void SensorManager_LoadMirror(void);

/* Resets FeRAM metadata header and clears the internal RAM mirror. */
void SensorManager_ResetFeramHeaderAndMirror(void);

/*
 * Updates the status of an existing sensor ID in RAM mirror and FeRAM if the
 * status differs, or adds a new entry when the ID is not present and
 * SENSOR_STATUS_PAIRING_BIT is set in status.
 * Returns 1 if the entry was updated or added, 0 otherwise.
 */
uint8_t SensorManager_UpdateSensorStatus(uint32_t id, uint8_t status);

#endif
