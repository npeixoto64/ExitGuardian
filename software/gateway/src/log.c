/**
 * @file log.c
 * @brief Blocking UART log helpers used for debug output.
 */
#include "log.h"
#include "stm8l15x_usart.h"

/**
 * @brief Send a NUL-terminated string over USART1 (blocking).
 *
 * @param str Pointer to the C string to transmit.
 */
void send_string(const char* str)
{
    while (*str) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData8(USART1, *str++);
    }
}

/**
 * @brief Send `"<reg_name> = 0xHH"` over USART1 (blocking).
 *
 * @param reg_name Label printed before the value.
 * @param value    8-bit value printed as two upper-case hex digits.
 */
void send_register_hex(const char* reg_name, uint8_t value)
{
    const char hex_chars[] = "0123456789ABCDEF";
    
    // Send register name
    send_string(reg_name);
    
    // Send " = 0x"
    send_string(" = 0x");
    
    // Send high nibble
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData8(USART1, hex_chars[(value >> 4) & 0x0F]);
    
    // Send low nibble
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData8(USART1, hex_chars[value & 0x0F]);
}