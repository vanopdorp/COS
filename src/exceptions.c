#include "print.h"
#include <stdint.h>

static const char* names[32] = {
    "Divide by zero (#DE)",         "Debug (#DB)",               "NMI",
    "Breakpoint (#BP)",             "Overflow (#OF)",            "BOUND range exceeded",
    "Invalid opcode (#UD)",         "Device not available (#NM)","Double fault (#DF)",
    "Coprocessor segment overrun",  "Invalid TSS (#TS)",         "Segment not present (#NP)",
    "Stack fault (#SS)",            "General protection (#GP)",  "Page fault (#PF)",
    "Reserved",                     "x87 FP (#MF)",              "Alignment check (#AC)",
    "Machine check (#MC)",          "SIMD FP (#XM)",             "Virtualization",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved", "Security", "Reserved"
};

void exception_handler(uint32_t vec, uint32_t err) {
    print("\nEXCEPTION: ");
    print(names[vec]);
    print(" | vector=");
    // eenvoudige dec-print (gebruik je eigen print_int als je die hebt)
    char buf[16]; int i=0; uint32_t x=vec;
    if (x==0) buf[i++]='0'; else { char tmp[16]; int j=0; while(x){ tmp[j++]= '0'+(x%10); x/=10; } while(j){ buf[i++]=tmp[--j]; } }
    buf[i]='\0'; print(buf);

    print(" | error=0x");
    char hex[9]; const char* d="0123456789ABCDEF";
    for (int k=7; k>=0; k--) { hex[7-k]= d[(err>>(k*4)) & 0xF]; }
    hex[8]='\0'; print(hex);
    print("\nHalting.\n");

    // stop veilig
    for(;;) { __asm__ __volatile__("hlt"); }
}

void isr16_handler() {
    print_color("EXCEPTION: x87 Floating Point Error (#MF)\n", 0x04);
    while (1); // Halt of recovery loop
}
