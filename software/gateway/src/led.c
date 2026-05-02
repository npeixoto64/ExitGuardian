#include "led.h"
#include "board.h"
#include "stm8l15x_gpio.h"

#define LEDB_PERIOD_MS   2000U  /* LED_B blink period */
#define LEDB_ON_MS        100U  /* LED_B on duration  */

// Function to handle periodic LED_B ON every 2s for 100ms
void periodic_ledb_tick(uint16_t now)
{
  static uint16_t last_tick = 0;
  static uint8_t  state = 0;
  if (!state && (uint16_t)(now - last_tick) >= LEDB_PERIOD_MS) {
    GPIO_SetBits(LED_B_PORT, LED_B_PIN); // Turn ON
    state = 1;
    last_tick = now;
  }
  if (state && (uint16_t)(now - last_tick) >= LEDB_ON_MS) {
    GPIO_ResetBits(LED_B_PORT, LED_B_PIN); // Turn OFF
    state = 0;
    // Do not update last_tick here
  }
}