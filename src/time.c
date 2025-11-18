#pragma once
#include "time.h"
#include "ports.h"
#include "print.h"
#include <stdint.h>
#include "string.h"

#define PIT_BASE 1193180


static volatile uint32_t g_ticks = 0;
static uint32_t g_hz = 100; // standaard frequentie
static int32_t timezone_offset = 0;   // standaard UTC
// Tijdstructuur: vergelijkbaar met tm_t in POSIX

// Initialiseer PIT timer
void init_timer(uint32_t hz) {
    if (hz == 0) hz = 100;
    g_hz = hz;
    uint32_t divisor = PIT_BASE / hz;
    outb(0x43, 0x36); // channel 0, mode 3
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

// Timer interrupt handler
void timer_handler(void) {
    g_ticks++;
}

// Periodieke PIT-mode
void pit_start_periodic(uint32_t hz) {
    uint32_t divisor = PIT_BASE / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

// One-shot PIT voor korte sleeps
void pit_oneshot(uint32_t ms) {
    uint32_t divisor = (PIT_BASE * ms) / 1000;
    if (divisor > 0xFFFF) divisor = 0xFFFF;
    outb(0x43, 0x30);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

// Hybride sleep
void sleep_ms(uint32_t ms) {
    if (ms <= 54) {
        pit_oneshot(ms);
        __asm__ __volatile__("sti; hlt");
        pit_start_periodic(g_hz);
    } else {
        uint32_t start = g_ticks;
        uint32_t wait_ticks = (ms * g_hz + 999) / 1000;
        while ((g_ticks - start) < wait_ticks) {
            __asm__ __volatile__("hlt");
        }
    }
}

void sleep(uint32_t ms) {
    sleep_ms(ms);
}


// Debug
uint64_t ticks_read(void) { return g_ticks; }
uint32_t timer_hz(void)   { return g_hz; }

// Tijdconversie en formattering (zoals eerder gegeven)
static const int days_in_month[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int is_leap(int year) {
    year += 1900;
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}
