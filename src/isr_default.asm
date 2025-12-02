; src/isr_default.asm
BITS 32
global isr_default

isr_default:
    pusha
    popa
    iret
