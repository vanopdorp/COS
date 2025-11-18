; src/isr_exceptions.asm
BITS 32
extern exception_handler  ; void exception_handler(uint32_t vec, uint32_t err);

%macro ISR_NOERR 1
global isr%1
isr%1:
    pusha
    push dword 0          ; err=0
    push dword %1         ; vec
    call exception_handler
    add esp, 8
    popa
    iret
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    pusha
    mov eax, [esp+32]     ; error code aangeduwd door CPU zit na pusha op +32
    push eax              ; err
    push dword %1         ; vec
    call exception_handler
    add esp, 8
    popa
    add esp, 4            ; eigen error code van de originele frame verwijderen
    iret
%endmacro

; 0..31
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
