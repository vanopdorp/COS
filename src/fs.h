#pragma once
#include "ports.h"
#include <stdint.h>
#include "string.h"

#define SECTOR_SIZE 2048
#define ATA_DATA       0x1F0
#define ATA_ERROR      0x1F1
#define ATA_SECCOUNT   0x1F2
#define ATA_LBA0       0x1F3
#define ATA_LBA1       0x1F4
#define ATA_LBA2       0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7
#define ATA_CMD_READ   0x20
#define ATA_CMD_WRITE  0x30
#define ATA_CMD_IDENTIFY 0xEC

uint32_t total_sectors = 0;
uint32_t file_table_start = 2;
uint32_t file_table_sectors = 0;

void outw(uint16_t port, uint16_t value) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

int ata_wait() {
    for (int i = 0; i < 100000; i++) {
        uint8_t status = inb(ATA_STATUS);
        if (!(status & 0x80) && (status & 0x08)) return 0;
    }
    return -1;
}

int ata_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA0, lba & 0xFF);
    outb(ATA_LBA1, (lba >> 8) & 0xFF);
    outb(ATA_LBA2, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_READ);
    if (ata_wait() != 0) return -1;
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = data >> 8;
    }
    return 0;
}

int ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA0, lba & 0xFF);
    outb(ATA_LBA1, (lba >> 8) & 0xFF);
    outb(ATA_LBA2, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_WRITE);
    if (ata_wait() != 0) return -1;
    for (int i = 0; i < 256; i++) {
        uint16_t data = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
        outw(ATA_DATA, data);
    }
    return 0;
}

void ata_identify() {
    outb(ATA_DRIVE, 0xA0);
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);
    if (ata_wait() != 0) return;
    uint16_t data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = inw(ATA_DATA);
    }
    total_sectors = ((uint32_t)data[61] << 16) | data[60];
    file_table_sectors = total_sectors / 100 * 3; // 3% of the file table for inodes
}

int fs_find_file_sector(const char* name) {
    uint8_t buffer[SECTOR_SIZE];
    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;
        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[32];
            int len = 0;
            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 31) naam[len++] = buffer[i++];
            naam[len] = '\0';
            if (buffer[i++] != ':') break;
            int sector = 0;
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9') sector = sector * 10 + (buffer[i++] - '0');
            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;
            if (i < SECTOR_SIZE) i++;
            if (strcmp(naam, name) == 1) return sector;
        }
    }
    return -1;
}
int fs_create_file(const char* name, const char* inhoud, uint32_t sector) {
    if (fs_write_text_sector(sector, inhoud) != 0) {
        print_color("Kon inhoud niet schrijven\n", 0x04);
        return -1;
    }

    char regel[64];
    char sector_str[12];
    int temp = sector, len = 0;
    do {
        sector_str[len++] = '0' + (temp % 10);
        temp /= 10;
    } while (temp > 0);
    sector_str[len] = '\0';
    for (int i = 0; i < len / 2; i++) {
        char t = sector_str[i];
        sector_str[i] = sector_str[len - 1 - i];
        sector_str[len - 1 - i] = t;
    }

    regel[0] = '\0';
    strcat(regel, name);
    strcat(regel, ":");
    strcat(regel, sector_str);
    strcat(regel, "\n");
    int regel_len = 0;
    while (regel[regel_len] != '\0') regel_len++;


    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        uint8_t buffer[SECTOR_SIZE];
        if (ata_read_sector(s, buffer) != 0) continue;

        int pos = 0;
        while (pos < SECTOR_SIZE) {
            if (buffer[pos] == 0) break;
            pos++;
        }

        // Is er genoeg ruimte?
        if (pos + regel_len < SECTOR_SIZE) {
            for (int i = 0; i < regel_len; i++) {
                buffer[pos + i] = regel[i];
            }
            int result = ata_write_sector(s, buffer);
            if (result == 0) {
            } else {
                print_color("Writing failed\n", 0x04);
            }
            return result;
        }
    }

    print_color("No disk space free\n", 0x04);
    return -1;
}
void fs_list_directory(const char* pad) {
    uint8_t buffer[SECTOR_SIZE];
    int pad_len = 0;
    while (pad[pad_len] != '\0') pad_len++; 

    int is_root = (pad_len == 1 && pad[0] == '/');  

    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue; 

        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[64];
            int len = 0;

            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 63) naam[len++] = buffer[i++];
            naam[len] = '\0';  
            if (buffer[i++] != ':') break;  

            int sector = 0;
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9') {
                sector = sector * 10 + (buffer[i++] - '0');
            }
            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;
            if (i < SECTOR_SIZE) i++;  

            int match = 0;
            if (is_root) {
                if (naam[0] == '/') match = 1;
            } else {
                if (strncmp(naam, pad, pad_len) == 1 && naam[pad_len] == '/') {
                    match = 1;
                }
            }


            if (match && sector > 0) {
                int extra_slash = 0;
                int start = pad_len + (is_root ? 0 : 1); 

                for (int j = start; naam[j] != '\0'; j++) {
                    if (naam[j] == '/') {
                        extra_slash = 1;  
                        break;
                    }
                }

                print_color(" - ", 0x0F);
                if (extra_slash) {
                    for (int j = start; naam[j] != '\0'; j++) {
                        if (naam[j] == '/') {
                            naam[j + 1] = '\0'; 
                            break;
                        }
                    }
                    if (naam[start] == '/') {
                        start++;
                    }
                    
                    print(&naam[start]);
                    print("/"); 
                } else {
                    print(&naam[start]);
                }
                print("\n");
            }
        }
    }
}

int fs_write_text_sector(uint32_t lba, const char* tekst) {
    uint8_t buffer[SECTOR_SIZE];
    memset(buffer, 0, SECTOR_SIZE);
    strncpy((char*)buffer, tekst, SECTOR_SIZE - 1);
    return ata_write_sector(lba, buffer);
}
char* fs_read_text_sector(uint32_t lba) {
    static uint8_t buffer[SECTOR_SIZE];
    if (ata_read_sector(lba, buffer) != 0) return "";
    return (char*)buffer;
}


char* fs_read_file(const char* name) {
    int sector = fs_find_file_sector(name);
    if (sector == -1) {
        print_color("\n", 0x04);
        return "File not found";
    }
    return fs_read_text_sector(sector);
    
}
// function to get all free file sectors on the disk
int get_free_sector() {
    static uint8_t used[100000] = {0};  

    uint8_t buffer[SECTOR_SIZE];

    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;

        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[32];
            int len = 0;

            // reading name
            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 31)
                naam[len++] = buffer[i++];
            naam[len] = '\0';

            if (buffer[i++] != ':') break;

            // reading sector number
            int sector = 0;
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9')
                sector = sector * 10 + (buffer[i++] - '0');

            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;
            if (i < SECTOR_SIZE) i++;

            if (sector > 0 && sector < total_sectors)
                used[sector] = 1;
        }
    }

    // 2. search for first free sector
    uint32_t start = file_table_start + file_table_sectors;
    for (uint32_t s = start; s < total_sectors; s++) {
        if (used[s] == 0) {
            return s;
        }
    }

    return -1;
}

void fs_init() {
    ata_identify();
    print_color("Total sectors: ", 0x0F);
    print_int(total_sectors);
    print("\nMax files: ");
    print_int(file_table_sectors * (SECTOR_SIZE / 25));
    print("\n");
}
int fs_file_exists(const char* name) {
    uint8_t buffer[SECTOR_SIZE];
    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;
        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[32];
            int len = 0;
            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 31) naam[len++] = buffer[i++];
            naam[len] = '\0';
            if (buffer[i++] != ':') break;
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9') i++;
            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;
            if (i < SECTOR_SIZE) i++;
            if (strcmp(naam, name) == 1) return 1;
        }
    }
    return 0;
}
int fs_is_directory(const char* pad) {
    uint8_t buffer[SECTOR_SIZE];
    
    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;  // Fault by reading the sector

        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[64];
            int len = 0;

            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 63) naam[len++] = buffer[i++];
            naam[len] = '\0';  

            if (buffer[i++] != ':') break; 

            int sector = 0;
            // Reads the sectors
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9') {
                sector = sector * 10 + (buffer[i++] - '0');
            }

            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;  // Skip the newline

            if (strcmp(naam, pad) == 0) {
                return 1;  // directory exist
            }
        }
    }

    return 0;  // Directory not found
