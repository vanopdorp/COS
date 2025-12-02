
#ifndef __PORTS_H__
#define __PORTS_H__
#include "types.h"
void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
uint16_t inw(uint16_t port); // ← voeg deze toe
static inline void outw(uint16_t port, uint16_t value);
#endif