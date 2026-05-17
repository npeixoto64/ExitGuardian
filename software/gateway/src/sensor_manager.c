#include "sensor_manager.h"

#include "feram.h"
#include "log.h"

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

/**
 * @brief Serialize the sensor manager metadata header into an 8-byte record.
 *
 * Lays out the header record as:
 *  - Byte 0: magic byte 0 (`SENSOR_MANAGER_HEADER_MAGIC0`, 'S')
 *  - Byte 1: magic byte 1 (`SENSOR_MANAGER_HEADER_MAGIC1`, 'M')
 *  - Byte 2: header version (`SENSOR_MANAGER_HEADER_VERSION`)
 *  - Byte 3: number of stored sensor entries
 *  - Bytes 4-5: reserved (set to 0)
 *  - Bytes 6-7: CRC-16/CCITT over bytes 0-5 (big-endian)
 *
 * The packed record is ready to be written to FeRAM at
 * `SENSOR_MANAGER_HEADER_ADDR`.
 *
 * @param[in]  count       Number of valid sensor entries currently stored.
 * @param[out] out_record  Destination buffer of size `SENSOR_MANAGER_RECORD_SIZE`
 *                         that receives the packed header bytes.
 */
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

/**
 * @brief Read and validate the sensor manager metadata header from FeRAM.
 *
 * Reads the 8-byte header record at `SENSOR_MANAGER_HEADER_ADDR` and verifies
 * its integrity by:
 *  - Recomputing the CRC-16/CCITT over bytes 0-5 and comparing against the
 *    big-endian CRC stored in bytes 6-7.
 *  - Checking the magic bytes (`SENSOR_MANAGER_HEADER_MAGIC0`,
 *    `SENSOR_MANAGER_HEADER_MAGIC1`) and version
 *    (`SENSOR_MANAGER_HEADER_VERSION`).
 *  - Ensuring the stored entry count does not exceed
 *    `SENSOR_MANAGER_MAX_SENSORS`.
 *
 * @param[out] count  On success, receives the number of valid sensor entries
 *                    reported by the header. Not modified on failure.
 *
 * @return 1U if the header is valid and @p count was written; 0U otherwise
 *         (CRC mismatch, bad magic/version, or out-of-range count).
 */
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

/**
 * @brief Validate the FeRAM metadata header, reinitializing it if corrupt.
 *
 * Attempts to read and verify the persisted header via
 * `sensor_manager_read_header()`. If the header is valid, the in-RAM entry
 * counter `g_sensor_count` is updated to reflect the persisted count. If the
 * header is invalid (CRC mismatch, bad magic/version, or out-of-range count),
 * `g_sensor_count` is reset to 0 and a fresh, empty header is written back to
 * FeRAM so subsequent operations start from a known-good state.
 *
 * @return 1U if the existing header was valid; 0U if it was invalid and a new
 *         empty header was written.
 */
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
            // ID found in mirror
            if (g_sensor_mirror[i].valid == 0U) {
                // Entry is currently invalid
                if (is_pairing_request == 0U) {
                    // No pairing request, return without update
                    send_string("\r\nID found, it's invalid, no pairing request -> entry remains invalid");
                    return SENSOR_NOT_UPDATED;
                }
                else {
                    // Pairing request for an already invalid entry, update and validate it
                    g_sensor_mirror[i].status = normalized_status;
                    g_sensor_mirror[i].valid = 1U;
                    sensor_manager_persist_record(i);
                    send_string("\r\nID found, it's invalid, pairing request -> entry validated");
                    return SENSOR_PAIRED;
                }
            }
            else {
                // Entry is currently valid
                if (is_unpairing_request != 0U) {
                    // Unpairing request for an already valid entry, invalidate it
                    g_sensor_mirror[i].status = normalized_status;
                    g_sensor_mirror[i].valid = 0U;
                    sensor_manager_persist_record(i);
                    send_string("\r\nID found, it's valid, unpairing request -> entry invalidated");
                    return SENSOR_UNPAIRED;
                }

                // No pairing/unpairing request, just update if status differs
                if (g_sensor_mirror[i].status != normalized_status) {
                    g_sensor_mirror[i].status = normalized_status;
                    sensor_manager_persist_record(i);
                    send_string("\r\nID found, it's valid, status updated");
                    return SENSOR_UPDATED;
                }

                send_string("\r\nID found, it's valid, no update needed");
                return SENSOR_UPDATED; // ID found but no update needed, still considered successful
            }
        }

        i++;
    }

    // ID not found in mirror, add new entry if pairing request and space available
    if ((is_pairing_request == 0U) || (g_sensor_count >= SENSOR_MANAGER_MAX_SENSORS)) {
        send_string("\r\nID not found, no pairing request or memory full, cannot add");
        return SENSOR_NOT_UPDATED;
    } else {
        g_sensor_mirror[g_sensor_count].id = id;
        g_sensor_mirror[g_sensor_count].status = normalized_status;
        g_sensor_mirror[g_sensor_count].valid = 1U;
        sensor_manager_persist_record(g_sensor_count);
        g_sensor_count++;
        sensor_manager_write_header(g_sensor_count);

        send_string("\r\nNew sensor paired");

        return SENSOR_PAIRED;
    }
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

uint8_t SensorManager_PairedCount(void)
{
    uint8_t i = 0U;
    uint8_t count = 0U;

    sensor_manager_ensure_mirror_loaded();

    while (i < g_sensor_count) {
        if (g_sensor_mirror[i].valid != 0U) {
            count++;
        }
        i++;
    }

    return count;
}
