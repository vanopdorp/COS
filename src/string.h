#ifndef __STRING_H__
#define __STRING_H__
#include <stddef.h>

// String manipulation
int strlen(const char s[]);
int strcmp(const char s1[], const char s2[]);
int strncmp(const char* a, const char* b, int l);
int startswith(char* a, char* b);
void strcpy(char a[], char b[]);
void reverse(char s[]);
void strtolower(char a[], char b[]);
char tolower(char a[]);
void subchar(char a[], char b[], int index);
void remchar(char* a, char* b);
void concat(char* a, char* b, char* c);
void concatc(char* a, char b, char* c);
void append(char s[], char n);
void backspace(char s[]);

// Conversion
void int_to_ascii(int n, char str[]);
int toHex(char c);
char* itoa(int num, char* str, int base);
// Memory
void memcpy(char* src, char* dst, int nbytes);
void* memcpy_std(void* dest, const void* src, size_t n);
void* memset(void* dest, int val, size_t len);
void int_to_string(int value, char* str);
int memcmp(const void *ptr1, const void *ptr2, size_t num);
void print_int(int top);
void strncpy(char* dest, const char* src, int n);
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);
char* strtok(char* str, const char* delim);
char* strcat(char* dest, const char* src);
char* strstr(const char* haystack, const char* needle);
char* strncat(char* dest, const char* src, size_t n);
#endif // __STRING_H__
