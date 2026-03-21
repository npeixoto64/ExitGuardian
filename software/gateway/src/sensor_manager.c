#include "sensor_manager.h"

#include "feram.h"

#define SENSOR_MANAGER_CRC_POLY 0x1021U
#define SENSOR_MANAGER_CRC_INIT 0xFFFFU

#define SENSOR_MANAGER_HEADER_MAGIC0 0x53U /* 'S' */
#define SENSOR_MANAGER_HEADER_MAGIC1 0x4DU /* 'M' */
#define SENSOR_MANAGER_HEADER_VERSION 0x01U
#define SENSOR_MANAGER_HEADER_ADDR 0x0000U

static SensorManagerEntry g_sensor_mirror[SENSOR_MANAGER_MAX_SENSORS];
static uint8_t g_sensor_mirror_loaded = 0U;
static uint8_t g_sensor_count = 0U;

static void sensor_manager_write_header(uint8_t count);
static void sensor_manager_clear_mirror(void);

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

static inline uint16_t sensor_manager_record_addr(uint8_t index)
{
    /* Record 0 is metadata header, sensor entries start at record 1. */
    return (uint16_t)(((uint16_t)index + 1U) << 3);
}

static void sensor_manager_pack_header(uint8_t count, uint8_t out_record[SENSOR_MANAGER_RECORD_SIZE])
{
    uint16_t crc = 0;

    out_record[0] = SENSOR_MANAGER_HEADER_MAGIC0;
    out_record[1] = SENSOR_MANAGER_HEADER_MAGIC1;
    out_record[2] = SENSOR_MANAGER_HEADER_VERSION;
    out_record[3] = count;
    out_record[4] = 0U;
    out_record[5] = 0U;

    crc = sensor_manager_crc16_ccitt(out_record, 6);
    out_record[6] = (uint8_t)(crc >> 8);
    out_record[7] = (uint8_t)crc;
}

static uint8_t sensor_manager_read_header(uint8_t* count)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];
    uint16_t stored_crc = 0;
    uint16_t computed_crc = 0;

    FeRAM_ReadBytes(SENSOR_MANAGER_HEADER_ADDR, record, SENSOR_MANAGER_RECORD_SIZE);

    stored_crc = ((uint16_t)record[6] << 8) | (uint16_t)record[7];
    computed_crc = sensor_manager_crc16_ccitt(record, 6);

    if (stored_crc != computed_crc) {
        return 0U;
    }

    if ((record[0] != SENSOR_MANAGER_HEADER_MAGIC0)
        || (record[1] != SENSOR_MANAGER_HEADER_MAGIC1)
        || (record[2] != SENSOR_MANAGER_HEADER_VERSION)
        || (record[3] > SENSOR_MANAGER_MAX_SENSORS)) {
        return 0U;
    }

    *count = record[3];
    return 1U;
}

static uint8_t sensor_manager_validate_header_and_reset_if_invalid(void)
{
    uint8_t count = 0U;

    if (sensor_manager_read_header(&count) != 0U) {
        g_sensor_count = count;
        return 1U;
    }

    g_sensor_count = 0U;
    sensor_manager_write_header(g_sensor_count);
    return 0U;
}

static void sensor_manager_write_header(uint8_t count)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];

    if (count > SENSOR_MANAGER_MAX_SENSORS) {
        count = SENSOR_MANAGER_MAX_SENSORS;
    }

    sensor_manager_pack_header(count, record);
    FeRAM_WriteBytes(SENSOR_MANAGER_HEADER_ADDR, record, SENSOR_MANAGER_RECORD_SIZE);
}

static void sensor_manager_clear_mirror(void)
{
    uint8_t i = 0;

    while (i < SENSOR_MANAGER_MAX_SENSORS) {
        g_sensor_mirror[i].id = 0U;
        g_sensor_mirror[i].status = 0U;
        g_sensor_mirror[i].valid = 0U;
        i++;
    }

    g_sensor_count = 0U;
}

static uint8_t sensor_manager_read_record_from_feram(uint8_t index, SensorManagerEntry* entry)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];
    uint16_t stored_crc = 0;
    uint16_t computed_crc = 0;

    FeRAM_ReadBytes(sensor_manager_record_addr(index), record, SENSOR_MANAGER_RECORD_SIZE);

    entry->id = ((uint32_t)record[0] << 24)
        | ((uint32_t)record[1] << 16)
        | ((uint32_t)record[2] << 8)
        | (uint32_t)record[3];
    entry->status = record[4];
    entry->valid = record[5];

    stored_crc = ((uint16_t)record[6] << 8) | (uint16_t)record[7];
    computed_crc = sensor_manager_crc16_ccitt(record, 6);

    return (stored_crc == computed_crc) ? 1U : 0U;
}

void SensorManager_LoadMirror(void)
{
    uint8_t i = 0;

    /* Clear entire mirror to ensure entries beyond g_sensor_count are zeroed.
     * This is defensive; as long as g_sensor_count is always respected,
     * entries beyond it should never be accessed. */
    sensor_manager_clear_mirror();

    if (sensor_manager_validate_header_and_reset_if_invalid()) {
        i = 0;
        while (i < g_sensor_count) {
            SensorManagerEntry entry;

            if (sensor_manager_read_record_from_feram(i, &entry)) {
                g_sensor_mirror[i] = entry;
            }
            i++;
        }
    }

    g_sensor_mirror_loaded = 1U;
}

void SensorManager_ResetFeramHeaderAndMirror(void)
{
    sensor_manager_clear_mirror();
    sensor_manager_write_header(0U);
    g_sensor_mirror_loaded = 1U;
}

static void sensor_manager_ensure_mirror_loaded(void)
{
    if (g_sensor_mirror_loaded == 0U) {
        SensorManager_LoadMirror();
    }
}

static void sensor_manager_pack_record(const SensorManagerEntry* entry, uint8_t out_record[SENSOR_MANAGER_RECORD_SIZE])
{
    uint16_t crc = 0;

    out_record[0] = (uint8_t)(entry->id >> 24);
    out_record[1] = (uint8_t)(entry->id >> 16);
    out_record[2] = (uint8_t)(entry->id >> 8);
    out_record[3] = (uint8_t)(entry->id);
    out_record[4] = entry->status;
    out_record[5] = entry->valid ? 1U : 0U;

    crc = sensor_manager_crc16_ccitt(out_record, 6);
    out_record[6] = (uint8_t)(crc >> 8);
    out_record[7] = (uint8_t)crc;
}

static void sensor_manager_persist_record(uint8_t index)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];

    sensor_manager_pack_record(&g_sensor_mirror[index], record);
    FeRAM_WriteBytes(sensor_manager_record_addr(index), record, SENSOR_MANAGER_RECORD_SIZE);
}

static void sensor_manager_write_record(uint8_t index, const SensorManagerEntry* entry)
{
    uint8_t record[SENSOR_MANAGER_RECORD_SIZE];

    if ((index >= SENSOR_MANAGER_MAX_SENSORS) || (entry == 0)) {
        return;
    }

    sensor_manager_ensure_mirror_loaded();

    g_sensor_mirror[index] = *entry;

    if (index >= g_sensor_count) {
        g_sensor_count = (uint8_t)(index + 1U);
    }

    sensor_manager_pack_record(entry, record);
    FeRAM_WriteBytes(sensor_manager_record_addr(index), record, SENSOR_MANAGER_RECORD_SIZE);
    sensor_manager_write_header(g_sensor_count);
}

static uint8_t sensor_manager_read_record(uint8_t index, SensorManagerEntry* entry)
{
    if ((index >= SENSOR_MANAGER_MAX_SENSORS) || (entry == 0)) {
        return 0U;
    }

    sensor_manager_ensure_mirror_loaded();

    *entry = g_sensor_mirror[index];

    return g_sensor_mirror[index].valid;
}

uint8_t SensorManager_UpdateSensorStatus(uint32_t id, uint8_t status)
{
    uint8_t i = 0;
    uint8_t is_pairing_request = SENSOR_STATUS_IS_PAIRING(status);
    uint8_t is_unpairing_request = SENSOR_STATUS_IS_UNPAIRING(status);
    uint8_t normalized_status = (uint8_t)(status & (uint8_t)~SENSOR_STATUS_PAIRING_MASK);

    sensor_manager_ensure_mirror_loaded();

    while (i < g_sensor_count) {
        if (g_sensor_mirror[i].id == id) {
            if (g_sensor_mirror[i].valid == 0U) {
                if (is_pairing_request == 0U) {
                    return 0U;
                }

                g_sensor_mirror[i].status = normalized_status;
                g_sensor_mirror[i].valid = 1U;
                sensor_manager_persist_record(i);
                return 1U;
            }

            if (is_unpairing_request != 0U) {
                g_sensor_mirror[i].status = normalized_status;
                g_sensor_mirror[i].valid = 0U;
                sensor_manager_persist_record(i);
                return 1U;
            }

            if (g_sensor_mirror[i].status != normalized_status) {
                g_sensor_mirror[i].status = normalized_status;
                sensor_manager_persist_record(i);
                return 1U;
            }

            return 1U;
        }

        i++;
    }

    if ((is_pairing_request == 0U)
        || (g_sensor_count >= SENSOR_MANAGER_MAX_SENSORS)) {
        return 0U;
    }

    g_sensor_mirror[g_sensor_count].id = id;
    g_sensor_mirror[g_sensor_count].status = normalized_status;
    g_sensor_mirror[g_sensor_count].valid = 1U;
    sensor_manager_persist_record(g_sensor_count);
    g_sensor_count++;
    sensor_manager_write_header(g_sensor_count);

    return 1U;
}

uint8_t SensorManager_AnyValidReedSwitchSet(void)
{
    uint8_t i = 0U;

    sensor_manager_ensure_mirror_loaded();

    while (i < g_sensor_count) {
        if ((g_sensor_mirror[i].valid != 0U)
            && (SENSOR_STATUS_IS_REED_SWITCH(g_sensor_mirror[i].status) != 0U)) {
            return 1U;
        }
        i++;
    }

    return 0U;
}
