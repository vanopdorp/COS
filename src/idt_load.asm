; idt_load.asm
extern idtp

global idt_load

section .text
idt_load:
    lidt [idtp]
    ret
