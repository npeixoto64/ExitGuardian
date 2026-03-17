#ifndef FERAM_H
#define FERAM_H

#include <stdint.h>

void FeRAM_WriteBytes(uint16_t mem_addr, uint8_t* pBuffer, uint8_t num_bytes);
void FeRAM_ReadBytes(uint16_t mem_addr, uint8_t* pBuffer, uint8_t num_bytes);

#endif
