#ifndef REED_H
#define REED_H

#include <stdint.h>

typedef enum {
  REED_EVT_NONE = 0,
  REED_EVT_OPENED,
  REED_EVT_CLOSED
} reed_event_t;

/* Called from EXTI5_IRQHandler in main.c; records the raw edge for debounce. */
void reed_isr(void);

/* Call once per main-loop iteration to process debounced reed events. */
void reed_handle(void);

/* Returns and clears the latest pending reed event. */
reed_event_t reed_take_event(void);

#endif /* REED_H */