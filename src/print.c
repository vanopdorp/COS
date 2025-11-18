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
#define COM1_PORT 0x3F8

void serial_write(char c) {
    // Wacht tot transmitter leeg is
    while (!(inb(COM1_PORT + 5) & 0x20));
    outb(COM1_PORT, c);
}

void serial_init() {
    outb(COM1_PORT + 1, 0x00);    // Disable interrupts
    outb(COM1_PORT + 3, 0x80);    // Enable DLAB
    outb(COM1_PORT + 0, 0x03);    // Baud rate divisor (low byte) = 38400
    outb(COM1_PORT + 1, 0x00);    // Baud rate divisor (high byte)
    outb(COM1_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7);    // Enable FIFO, clear them, 14-byte threshold
    outb(COM1_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}


void print(const char* msg) {
    char* vga = (char*)0xB8000;
    const int width = 80;

    for (int i = 0; msg[i]; i++) {
        char c = msg[i];

        // Seriële uitvoer
        serial_write(c);

        // VGA uitvoer
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



void clear_screen() {
    char* vga = (char*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga[i * 2] = ' ';
        vga[i * 2 + 1] = 0x07;  // standaard kleur: lichtgrijs op zwart
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_color(const char* msg, unsigned char color) {
    char* vga = (char*)0xB8000;
    const int width = 80;

    for (int i = 0; msg[i]; i++) {
        char c = msg[i];

        // Seriële uitvoer
        serial_write(c);

        // VGA uitvoer
        if (c == '\n') {
            cursor_x = 0;
            cursor_y++;
            continue;
        }

        int pos = (cursor_y * width + cursor_x) * 2;
        vga[pos] = c;
        vga[pos + 1] = color;

        cursor_x++;
        if (cursor_x >= width) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    update_cursor(cursor_x, cursor_y);
}

void back_cursor(int count) {
    if (count <= 0) return;

    char* vga = (char*)0xB8000;
    const int width = 80;

    // Verwijder de 'count' aantal tekens
    for (int i = 0; i < count; i++) {
        if (cursor_x == 0 && cursor_y == 0) return;  // Als we op de eerste positie zijn, stop dan

        if (cursor_x == 0) {  // Als we aan het begin van de lijn zijn, ga dan naar de vorige lijn
            cursor_y--;
            cursor_x = width - 1;
        } else {
            cursor_x--;
        }

        // Wis het teken op de huidige cursorlocatie
        int pos = (cursor_y * width + cursor_x) * 2;
        vga[pos] = ' ';
        vga[pos + 1] = 0x07;  // Standaard kleur: lichtgrijs op zwart
    }

    // Zorg ervoor dat de cursor zich aanpast op de juiste positie
    update_cursor(cursor_x, cursor_y);
}
void print_hex(uint8_t byte) {
    const char* hex_chars = "0123456789ABCDEF";
    char hex[3];

    hex[0] = hex_chars[(byte >> 4) & 0x0F]; // High nibble
    hex[1] = hex_chars[byte & 0x0F];        // Low nibble
    hex[2] = '\0';

    print(hex);
}
