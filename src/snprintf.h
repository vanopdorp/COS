#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"

static int int_to_str(int num, char *out, int base, int is_unsigned) {
    char digits[] = "0123456789abcdef";
    char temp[32];
    int len = 0;

    if (!is_unsigned && num < 0) {
        *out++ = '-';
        num = -num;
    }

    unsigned int n = (unsigned int)num;
    if (n == 0) {
        temp[len++] = '0';
    } else {
        while (n > 0) {
            temp[len++] = digits[n % base];
            n /= base;
        }
    }

    for (int i = len - 1; i >= 0; i--) {
        *out++ = temp[i];
    }

    return len + (!is_unsigned && num < 0 ? 1 : 0);
}

int snprintf(char *buffer, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    size_t written = 0;
    const char *ptr = format;

    while (*ptr != '\0' && written < size - 1) {
        if (*ptr == '%') {
            ptr++;
            if (*ptr == 's') {
                const char *str = va_arg(args, const char*);
                while (*str && written < size - 1) {
                    buffer[written++] = *str++;
                }
            } else if (*ptr == 'd' || *ptr == 'i') {
                int num = va_arg(args, int);
                char temp[32];
                int len = int_to_str(num, temp, 10, 0);
                for (int i = 0; i < len && written < size - 1; i++) {
                    buffer[written++] = temp[i];
                }
            } else if (*ptr == 'u') {
                unsigned int num = va_arg(args, unsigned int);
                char temp[32];
                int len = int_to_str(num, temp, 10, 1);
                for (int i = 0; i < len && written < size - 1; i++) {
                    buffer[written++] = temp[i];
                }
            } else if (*ptr == 'x') {
                unsigned int num = va_arg(args, unsigned int);
                char temp[32];
                int len = int_to_str(num, temp, 16, 1);
                for (int i = 0; i < len && written < size - 1; i++) {
                    buffer[written++] = temp[i];
                }
            } else if (*ptr == 'c') {
                char c = (char)va_arg(args, int);
                buffer[written++] = c;
            } else if (*ptr == '%') {
                buffer[written++] = '%';
            }
            ptr++;
        } else {
            buffer[written++] = *ptr++;
        }
    }

    buffer[written] = '\0';
    va_end(args);
    return written;
}
