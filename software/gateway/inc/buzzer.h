#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

typedef enum {
  BUZZER_MODE_OFF = 0,
  BUZZER_MODE_LONG,   /* one-shot 2 s tone */
  BUZZER_MODE_BEEP    /* short beep every 1 s */
} buzzer_mode_t;

/* Call every main-loop iteration with the current ms tick and desired mode.
 * - OFF  : silent.
 * - LONG : on entry, tone for 2 s then auto-stops; staying in LONG does not
 *          retrigger. To replay, switch to OFF (or BEEP) and back to LONG.
 * - BEEP : 100 ms tone every 1 s while mode is selected.
 */
void buzzer_handle(uint16_t now, buzzer_mode_t mode);

#endif /* BUZZER_H */
