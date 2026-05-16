#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

typedef enum {
  BUTTON_EVT_NONE = 0,
  BUTTON_EVT_SHORT_PRESS,         /* released within <= 2 s (FR_014/FR_017) */
  BUTTON_EVT_LONG_PRESS_DETECTED, /* held for >= 5 s (FR_024)              */
  BUTTON_EVT_LONG_PRESS_RELEASED  /* released after >= 5 s   (FR_024)       */
} button_event_t;

/* Called from EXTI4_IRQHandler in main.c; records the raw edge for debounce. */
void button_isr(void);

/* Call once per main-loop iteration to process debounced button events. */
void button_handle(void);

/* Returns and clears the latest pending button event. */
button_event_t button_take_event(void);

#endif /* BUTTON_H */
