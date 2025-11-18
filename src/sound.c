#include "ports.h"
#include "types.h"
void play_sound(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Command byte: channel 2, lobyte/hibyte, mode 3 (square wave)
    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(divisor & 0xFF));       // Low byte
    outb(0x42, (uint8_t)((divisor >> 8) & 0xFF)); // High byte

    // Enable speaker
    uint8_t tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

void stop_sound() {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}
