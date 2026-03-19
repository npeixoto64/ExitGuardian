#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>

#define SENSOR_MANAGER_RECORD_SIZE 8
#define SENSOR_MANAGER_MAX_SENSORS 100U

/*
 * Status layout (MSB -> LSB):
 * [BAT SoC(4 bits), 0, pairing, reed-switch, valid]
 */
#define SENSOR_STATUS_VALID_BIT            0U
#define SENSOR_STATUS_REED_SWITCH_BIT      1U
#define SENSOR_STATUS_PAIRING_BIT          2U
#define SENSOR_STATUS_RESERVED_BIT         3U
#define SENSOR_STATUS_BAT_SOC_SHIFT        4U

#define SENSOR_STATUS_VALID_MASK           (1U << SENSOR_STATUS_VALID_BIT)
#define SENSOR_STATUS_REED_SWITCH_MASK     (1U << SENSOR_STATUS_REED_SWITCH_BIT)
#define SENSOR_STATUS_PAIRING_MASK         (1U << SENSOR_STATUS_PAIRING_BIT)
#define SENSOR_STATUS_RESERVED_MASK        (1U << SENSOR_STATUS_RESERVED_BIT)
#define SENSOR_STATUS_BAT_SOC_MASK         (0x0FU << SENSOR_STATUS_BAT_SOC_SHIFT)

#define SENSOR_STATUS_PACK(bat_soc, pairing, reed_switch, valid) \
    (uint8_t)((((uint8_t)(bat_soc) & 0x0FU) << SENSOR_STATUS_BAT_SOC_SHIFT) \
        | (((pairing) ? 1U : 0U) << SENSOR_STATUS_PAIRING_BIT) \
        | (((reed_switch) ? 1U : 0U) << SENSOR_STATUS_REED_SWITCH_BIT) \
        | (((valid) ? 1U : 0U) << SENSOR_STATUS_VALID_BIT))

#define SENSOR_STATUS_GET_BAT_SOC(status) \
    (uint8_t)(((uint8_t)(status) & SENSOR_STATUS_BAT_SOC_MASK) >> SENSOR_STATUS_BAT_SOC_SHIFT)
#define SENSOR_STATUS_IS_PAIRING(status) \
    (uint8_t)((((uint8_t)(status) & SENSOR_STATUS_PAIRING_MASK) != 0U) ? 1U : 0U)
#define SENSOR_STATUS_IS_REED_SWITCH(status) \
    (uint8_t)((((uint8_t)(status) & SENSOR_STATUS_REED_SWITCH_MASK) != 0U) ? 1U : 0U)
#define SENSOR_STATUS_IS_VALID(status) \
    (uint8_t)((((uint8_t)(status) & SENSOR_STATUS_VALID_MASK) != 0U) ? 1U : 0U)

typedef struct {
    uint32_t id;
    uint8_t status;
} SensorManagerEntry;

/* Loads all sensor records from FeRAM into the internal RAM mirror. */
void SensorManager_LoadMirror(void);

/* Discards the current RAM mirror and restores it from FeRAM. */
void SensorManager_RestoreMirrorFromFeRAM(void);

/* Validates the first 8-byte FeRAM header and resets sensor count to zero if invalid. */
uint8_t SensorManager_ValidateHeaderAndResetIfInvalid(void);

void SensorManager_PackRecord(const SensorManagerEntry* entry, uint8_t out_record[SENSOR_MANAGER_RECORD_SIZE]);
void SensorManager_WriteRecord(uint8_t index, const SensorManagerEntry* entry);
uint8_t SensorManager_ReadRecord(uint8_t index, SensorManagerEntry* entry);

void SensorManager_WriteList(const SensorManagerEntry* entries, uint8_t count);
uint8_t SensorManager_ReadList(SensorManagerEntry* entries, uint8_t count);

/*
 * Updates the status of an existing sensor ID in RAM mirror and FeRAM if the
 * status differs, or adds a new entry when the ID is not present and
 * SENSOR_STATUS_PAIRING_BIT is set in status.
 * Returns 1 if the entry was updated or added, 0 otherwise.
 */
uint8_t SensorManager_UpdateOrAddIfPairing(uint32_t id, uint8_t status);

/*
 * Clears the SENSOR_STATUS_VALID_BIT of a sensor by ID in RAM mirror and FeRAM.
 * Returns 1 if the sensor ID exists, 0 otherwise.
 */
uint8_t SensorManager_ClearValidBitById(uint32_t id);

#endif
