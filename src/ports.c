
#include "ports.h"
void outb(unsigned short port, unsigned char data){
asm volatile("outb %0, %1": :"a"(data), "dN"(port));
} 

unsigned char inb(unsigned short port){
	unsigned short ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outw(uint16_t port, uint16_t value) {
    __asm__ __volatile__ (
        "outw %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}
