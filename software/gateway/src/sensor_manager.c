#include "sensor_manager.h"

#include "feram.h"

#define SENSOR_MANAGER_CRC_POLY 0x1021U
#define SENSOR_MANAGER_CRC_INIT 0xFFFFU

static uint16_t sensor_manager_crc16_ccitt(const uint8_t* data, uint8_t len)
{
    uint16_t crc = SENSOR_MANAGER_CRC_INIT;
    uint8_t i = 0;

    while (i < len) {
        uint8_t bit = 0;

        crc ^= (uint16_t)data[i] << 8;
        while (bit < 8) {
            if (crc & 0x8000U) {
                crc = (uint16_t)((crc << 1) ^ SENSOR_MANAGER_CRC_POLY);
            } else {
                crc <<= 1;
            }
            bit++;
        }
        i++;
    }

    return crc;
}

static uint16_t sensor_manager_record_addr(uint16_t base_addr, uint8_t index)
{
    return (uint16_t)(base_addr + ((uint16_t)index * SENSOR_MANAGER_RECORD_SIZE));
}

void SensorManager_PackRecord(const SensorManagerEntry* entry, uint8_t out_record[SENSOR_MANAGER_RECORD_SIZE])
{
    uint16_t crc = 0;

    out_record[0] = (uint8_t)(entry->id >> 24);
    out_record[1] = (uint8_t)(entry->id >> 16);
    out_record[2] = (uint8_t)(entry->id >> 8);
    out_record[3] = (uint8_t)(entry->id);
    out_record[4] = entry->status;
    out_record[5] = 0;

    crc = sensor_manager_crc16_ccitt(out_record, 6);
    out_record[6] = (uint8_t)(crc >> 8);
    out_record[7] = (uint8_t)crc;
}

void SensorManager_WriteRecord(uint16_t base_addr, uint8_t index, const SensorManagerEntry* entry)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];

    SensorManager_PackRecord(entry, record);
    FeRAM_WriteBytes(sensor_manager_record_addr(base_addr, index), record, SENSOR_MANAGER_RECORD_SIZE);
}

uint8_t SensorManager_ReadRecord(uint16_t base_addr, uint8_t index, SensorManagerEntry* entry)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];
    uint16_t stored_crc = 0;
    uint16_t computed_crc = 0;

    FeRAM_ReadBytes(sensor_manager_record_addr(base_addr, index), record, SENSOR_MANAGER_RECORD_SIZE);

    entry->id = ((uint32_t)record[0] << 24)
        | ((uint32_t)record[1] << 16)
        | ((uint32_t)record[2] << 8)
        | (uint32_t)record[3];
    entry->status = record[4];

    stored_crc = ((uint16_t)record[6] << 8) | (uint16_t)record[7];
    computed_crc = sensor_manager_crc16_ccitt(record, 6);

    return (stored_crc == computed_crc) ? 1U : 0U;
}

void SensorManager_WriteList(uint16_t base_addr, const SensorManagerEntry* entries, uint8_t count)
{
    uint8_t i = 0;

    while (i < count) {
        SensorManager_WriteRecord(base_addr, i, &entries[i]);
        i++;
    }
}

uint8_t SensorManager_ReadList(uint16_t base_addr, SensorManagerEntry* entries, uint8_t count)
{
    uint8_t i = 0;
    uint8_t valid_count = 0;

    while (i < count) {
        if (SensorManager_ReadRecord(base_addr, i, &entries[i])) {
            valid_count++;
        }
        i++;
    }

    return valid_count;
}
