; src/irq0.asm
BITS 32
global irq0_stub
extern timer_handler

irq0_stub:
    push ds
    push es
    push fs
    push gs
    mov ax, 0x18          ; JOUW data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pusha
    call timer_handler
    popa

    pop gs
    pop fs
    pop es
    pop ds

    mov al, 0x20
    out 0x20, al
    iret
