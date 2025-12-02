#ifndef PRINT_H
#define PRINT_H

#include <stddef.h>
#include "types.h"
void print(const char* msg);
void print_color(const char* msg, unsigned char color);
void clear_screen();
void back_cursor(int count);
void serial_init();
void* memset(void* dest, int val, size_t len);
void printf(const char* format, ...);
void print_hex(uint8_t byte);
#endif
