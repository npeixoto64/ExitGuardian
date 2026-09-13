/**
 * @file log.h
 * @brief Simple blocking UART log helpers for debug output.
 */
#ifndef LOG_H
#define LOG_H

#include <stdint.h>

/**
 * @brief Send a NUL-terminated string over USART1 (blocking).
 *
 * @param str Pointer to the C string to transmit.
 */
void send_string(const char* str);

// /**
//  * @brief Send `"<reg_name> = 0xHH"` over USART1 (blocking).
//  *
//  * @param reg_name Label printed before the value.
//  * @param value    8-bit value printed as two upper-case hex digits.
//  */
// void send_register_hex(const char* reg_name, uint8_t value);

/**
 * @brief Send a single byte as two upper-case hex digits over USART1 (blocking).
 *
 * @param value 8-bit value printed as two upper-case hex digits.
 */
void send_hex_byte(uint8_t value);

/**
 * @brief Send an 8-bit unsigned integer as a decimal string over USART1 (blocking).
 *
 * @param value 8-bit unsigned integer to transmit.
 */
void send_uint8_t(uint8_t value);

#endif