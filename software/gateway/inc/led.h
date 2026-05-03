#ifndef LED_H
#define LED_H

#include <stdint.h>

typedef enum {
  LED_MODE_OFF = 0,
  LED_MODE_ON,
  LED_MODE_FLASH
} led_mode_t;

void led_b_handle(uint16_t now);
void led_r_handle(uint16_t now, led_mode_t mode);
void led_y_handle(uint16_t now, led_mode_t mode);

#endif /* LED_H */
