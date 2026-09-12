#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

void mcu_init(void);
void board_pre_init_tx(void);
void board_pre_init_rx(void);
void board_init(void);
uint16_t board_read_pd3_adc(void);

#endif
