#ifndef KB_H
#define KB_H
#define INPUT_MAX 256
static char input_buffer[INPUT_MAX];
static unsigned char input_index = 0;

char* get_input_buffer() {
    return input_buffer;
}
#include "print.h"
#include "ports.h"
#include "string.h"
const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', // 0 - 9
   '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', // 10 - 19
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,   // 20 - 29
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', // 30 - 39
  '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',  // 40 - 49
   'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,         // 50 - 59
    // rest is 0
};

const char scancode_shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',  // 0 - 9
   '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', // 10 - 19
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,   // 20 - 29
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', // 30 - 39
   '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N',  // 40 - 49
   'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0,         // 50 - 59
   // rest is 0
};
char* readStr() {
    input_index = 0;
    unsigned char reading = 1;
    unsigned char shift = 0;

    while (reading) {
        if (inb(0x64) & 0x1) {
            unsigned char scancode = inb(0x60);

            // Shift keys
            if (scancode == 0x2A || scancode == 0x36) {
                shift = 1;
                continue;
            }
            if (scancode == 0xAA || scancode == 0xB6) {
                shift = 0;
                continue;
            }

            // Ignore key releases
            if (scancode & 0x80) continue;

            char ch = shift ? scancode_shift_map[scancode] : scancode_map[scancode];

            if (ch == '\n') {
                reading = 0;
                print("\n");
            } else if (ch == '\b') {
                if (input_index > 0) {
                    input_index--;
                    input_buffer[input_index] = '\0';
                    back_cursor(1);
                }
            } else if (ch != 0 && input_index < INPUT_MAX - 1) {
                input_buffer[input_index++] = ch;
                input_buffer[input_index] = '\0';
                char str[2] = {ch, '\0'};
                print(str);
            }
        }
    }

    return input_buffer;
}

#endif // KB_H