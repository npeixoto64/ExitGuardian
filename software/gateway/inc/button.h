#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

/* Called from EXTI4_IRQHandler in main.c; records the raw edge for debounce. */
void button_isr(void);

/* Call once per main-loop iteration to process debounced button events. */
void button_handle(void);

#endif /* BUTTON_H */
