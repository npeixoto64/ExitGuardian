#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <stdint.h>

#define SENSOR_MANAGER_RECORD_SIZE 8

typedef struct {
    uint32_t id;
    uint8_t status;
} SensorManagerEntry;

void SensorManager_PackRecord(const SensorManagerEntry* entry, uint8_t out_record[SENSOR_MANAGER_RECORD_SIZE]);
void SensorManager_WriteRecord(uint8_t index, const SensorManagerEntry* entry);
uint8_t SensorManager_ReadRecord(uint8_t index, SensorManagerEntry* entry);

void SensorManager_WriteList(const SensorManagerEntry* entries, uint8_t count);
uint8_t SensorManager_ReadList(SensorManagerEntry* entries, uint8_t count);

#endif
