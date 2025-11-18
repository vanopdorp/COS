#include "idt.h"

#define IDT_ENTRIES 256

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_reg;

extern void idt_flush(uint32_t); // from idt_flush.asm

// idt.c
void set_idt_gate(int n, uint32_t handler) {
    idt[n].base_low  = handler & 0xFFFF;
    idt[n].sel       = 0x10;   // JOUW code segment uit GDT (niet 0x08)
    idt[n].always0   = 0;
    idt[n].flags     = 0x8E;   // present, DPL=0, 32-bit interrupt gate
    idt[n].base_high = (handler >> 16) & 0xFFFF;
}

void load_idt() {
    idt_reg.base = (uint32_t)&idt;
    idt_reg.limit = sizeof(idt) - 1;
    idt_flush((uint32_t)&idt_reg);
}
