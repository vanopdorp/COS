#include "heap.h"
#include <stddef.h>

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    if (d == s || n == 0) {
        return dest;
    }

    if (d < s) {
        // Geen overlap of veilige richting: kopieer vooruit
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        // Overlap: kopieer achteruit
        for (size_t i = n; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }

    return dest;
}

#define HEAP_START 0x100000  // 1MB

typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
} Block;

#define BLOCK_SIZE sizeof(Block)

#define HEAP_SIZE 0x100000
static uint8_t heap_area[HEAP_SIZE];
static Block* free_list = NULL;

void heap_init() {
    free_list = (Block*)heap_area;
    free_list->size = HEAP_SIZE - BLOCK_SIZE;
    free_list->free = 1;
    free_list->next = NULL;
}

void* kmalloc(size_t size) {
    Block* curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + BLOCK_SIZE) {
                Block* new_block = (Block*)((uint8_t*)curr + BLOCK_SIZE + size);
                new_block->size = curr->size - size - BLOCK_SIZE;
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void*)((uint8_t*)curr + BLOCK_SIZE);
        }
        curr = curr->next;
    }
    return NULL; // geen ruimte
}

void kfree(void* ptr) {
    if (!ptr) return;
    Block* block = (Block*)((uint8_t*)ptr - BLOCK_SIZE);
    block->free = 1;

    // optioneel: merge met volgende vrije blokken
    if (block->next && block->next->free) {
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
    }
}
