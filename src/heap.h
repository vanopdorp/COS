#pragma once
#include <stddef.h>
#include <stdint.h>
void* memmove(void* dest, const void* src, size_t n);
void heap_init();
void* kmalloc(size_t size);
void kfree(void* ptr);