/**
 * @file feram.h
 * @brief Minimal I2C driver for the on-board FRAM non-volatile memory.
 */
#ifndef FERAM_H
#define FERAM_H

#include <stdint.h>

/**
 * @brief Write a buffer to the FRAM device.
 *
 * Issues a single I2C write transaction containing the 16-bit memory address
 * followed by @p num_bytes payload bytes.
 *
 * @param mem_addr  16-bit FRAM byte address to start writing at.
 * @param pBuffer   Pointer to the source buffer.
 * @param num_bytes Number of bytes to write.
 */
void FeRAM_WriteBytes(uint16_t mem_addr, uint8_t* pBuffer, uint8_t num_bytes);

/**
 * @brief Read a buffer from the FRAM device.
 *
 * Issues an I2C write of the 16-bit memory address followed by a repeated
 * start and read of @p num_bytes bytes.
 *
 * @param mem_addr  16-bit FRAM byte address to start reading from.
 * @param pBuffer   Destination buffer.
 * @param num_bytes Number of bytes to read.
 */
void FeRAM_ReadBytes(uint16_t mem_addr, uint8_t* pBuffer, uint8_t num_bytes);

#endif
