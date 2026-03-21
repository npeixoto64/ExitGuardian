#include "log.h"
#include "stm8l15x_usart.h"

void send_string(const char* str)
{
    while (*str) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData8(USART1, *str++);
    }
}

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