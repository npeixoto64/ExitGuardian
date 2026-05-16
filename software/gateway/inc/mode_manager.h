#ifndef MODE_MANAGER_H
#define MODE_MANAGER_H

#include <stdint.h>

/* Gateway operating modes (FR_005). */
typedef enum {
  MODE_WAITING_FOR_CONFIG = 0,    /* No sensors paired (FR_007, FR_010).        */
  MODE_MONITORING,                /* >= 1 sensor paired (FR_008, FR_011, FR_012). */
  MODE_PAIRING_UNPAIRING,         /* Pair/unpair window open (FR_015, FR_016).  */
  MODE_WAITING_FOR_FACTORY_RESET, /* Waiting for factory reset (FR_024, FR_025). */
} mode_t;

/* Initialise mode based on paired-sensor count restored from NVM (FR_006).
 * Must be called after SensorManager_LoadMirror() and board_init(). */
void mode_manager_init(void);

/* Main-loop tick. Drives mode-dependent indicators and timeouts (FR_004,
 * FR_007, FR_008, FR_010, FR_015, FR_022, FR_023). */
void mode_manager_handle(uint16_t now);

/* Returns the current operating mode. */
mode_t mode_manager_get(void);

/* Button events from button_handle() (FR_014, FR_017, FR_024, FR_025). */
void mode_manager_on_short_press(void);
void mode_manager_on_long_press_detected(void);
void mode_manager_on_long_press_released(void);

/* RF packet from cc1101_recv_msg(); routes to pair/unpair logic when in
 * MODE_PAIRING_UNPAIRING (FR_018-FR_021), otherwise to the alert pipeline
 * (FR_012). */
void mode_manager_on_sensor_packet(uint32_t sensor_id, uint8_t status);

/* Factory RESET (FR_009, FR_024, FR_026). Clears NVM, returns to
 * MODE_WAITING_FOR_CONFIG. */
void mode_manager_factory_reset(void);

#endif /* MODE_MANAGER_H */
