#ifndef REED_H
#define REED_H

/* Called from EXTI5_IRQHandler in main.c; records the raw edge for debounce. */
void reed_isr(void);

/* Call once per main-loop iteration to process debounced reed events. */
void reed_handle(void);

#endif /* REED_H */