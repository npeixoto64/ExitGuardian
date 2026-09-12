#include "device_id.h"
#include "app_config.h"
#include "log.h"

#define STM8_UID_BASE_ADDR 0x4865u
#define STM8_UID_LEN       12u

#define FNV_OFFSET_BASIS   0x811C9DC5UL
#define FNV_PRIME          0x01000193UL

static uint32_t calculate_hash(const uint8_t *data, uint8_t len)
{
    uint32_t hash = FNV_OFFSET_BASIS;
    uint8_t i;

    for (i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= FNV_PRIME;
    }

    return hash;
}

uint32_t device_id_get_hash(void)
{
    const volatile uint8_t *uid = (const volatile uint8_t *)STM8_UID_BASE_ADDR;
    uint32_t hash = calculate_hash((const uint8_t *)uid, STM8_UID_LEN);

#ifdef DEBUG
    uint8_t i;
    send_string("\r\n\r\nSTM8 UID:");

    for (i = 0; i < STM8_UID_LEN; ++i) {
        send_string(" ");
        send_hex_byte(uid[i]);
    }

    send_string("\r\nSTM8 UID hash: ");
    send_hex_byte((hash >> 24) & 0xFF);
    send_hex_byte((hash >> 16) & 0xFF);
    send_hex_byte((hash >> 8) & 0xFF);
    send_hex_byte(hash & 0xFF);
    send_string("\r\n");
#endif

    return hash;
}
