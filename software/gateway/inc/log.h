#ifndef LOG_H
#define LOG_H

#include <stdint.h>

void send_string(const char* str);
void send_register_hex(const char* reg_name, uint8_t value);

#endif