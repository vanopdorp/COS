__attribute__((section(".multiboot"), used))
unsigned int multiboot_header[] = {
    0x1BADB002,         // magic
    0x00000003,         // flags
    0xE4524FFB          // checksum = -(magic + flags)
};
#include <stdint.h>
#include "ports.h"

int cursor_x = 0;
int cursor_y = 0;

void update_cursor(int x, int y) {
    uint16_t pos = y * 80 + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));

}

void print(const char* msg) {
    char* vga = (char*)0xB8000;
    const int width = 80;

    for (int i = 0; msg[i]; i++) {
        char c = msg[i];


        // VGA output
        if (c == '\n') {
            cursor_x = 0;
            cursor_y++;
            continue;
        }

        int pos = (cursor_y * width + cursor_x) * 2;
        vga[pos] = c;
        vga[pos + 1] = 0x07;

        cursor_x++;
        if (cursor_x >= width) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    update_cursor(cursor_x, cursor_y);
}

void kernel_main() {
    print("hello from a os\n");
    while (1==1) {

    }
}