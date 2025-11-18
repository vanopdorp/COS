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
    file_table_sectors = total_sectors / 100 * 3; // 1% van schijf voor bestandstabel
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

    // Bouw regel "naam:sector\n"
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

    // Zoek vrije plek in bestandstabel
    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        uint8_t buffer[SECTOR_SIZE];
        if (ata_read_sector(s, buffer) != 0) continue;

        // Zoek naar eerste vrije positie: scan tot eerste 0-byte
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
    while (pad[pad_len] != '\0') pad_len++;  // Bereken de lengte van het opgegeven pad

    int is_root = (pad_len == 1 && pad[0] == '/');  // Controleer of we de root directory proberen

    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;  // Fout bij het lezen van sector, ga verder

        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[64];
            int len = 0;

            // Lees de bestandsnaam tot aan ':' of tot we 63 tekens hebben
            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 63) naam[len++] = buffer[i++];
            naam[len] = '\0';  // Null-terminate de naam
            if (buffer[i++] != ':') break;  // Verwacht een ':' na de naam, anders breek de loop

            int sector = 0;
            // Lees de sector
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9') {
                sector = sector * 10 + (buffer[i++] - '0');
            }

            // Zoek de volgende newline, overslaan
            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;
            if (i < SECTOR_SIZE) i++;  // Verwerk de newline

            // Controleer of de naam overeenkomt met het pad
            int match = 0;
            if (is_root) {
                // Rootdirectory ("/") is speciaal, we checken enkel of het pad begint met "/"
                if (naam[0] == '/') match = 1;
            } else {
                // Voor subdirectories moeten we controleren of de naam begint met het pad + "/"
                if (strncmp(naam, pad, pad_len) == 1 && naam[pad_len] == '/') {
                    match = 1;
                }
            }

            // Als de naam overeenkomt en de sector geldig is, laat de naam zien
            if (match && sector > 0) {
                int extra_slash = 0;
                int start = pad_len + (is_root ? 0 : 1);  // Start van de naam, afhankelijk van of het root is

                // Zoek naar een '/' in de naam (dit betekent dat we met een directory te maken hebben)
                for (int j = start; naam[j] != '\0'; j++) {
                    if (naam[j] == '/') {
                        extra_slash = 1;  // Dit is een directory
                        break;
                    }
                }

                // Print de naam van het bestand of de subdirectory
                print_color(" - ", 0x0F);
                if (extra_slash) {
                    // Subdirectory gevonden, toon de naam tot aan de eerste '/'
                    for (int j = start; naam[j] != '\0'; j++) {
                        if (naam[j] == '/') {
                            naam[j + 1] = '\0';  // Truncate de naam bij de eerste '/'
                            break;
                        }
                    }

                    // Vermijd dubbele slashes in de naam
                    if (naam[start] == '/') {
                        start++;  // Verwijder de extra '/' aan het begin van de naam
                    }
                    
                    print(&naam[start]);
                    print("/");  // Voeg een "/" toe om het als directory aan te geven
                } else {
                    // Gewoon bestand
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
    static uint8_t buffer[SECTOR_SIZE]; // let op: static!
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
int get_free_sector() {
    static uint8_t used[100000] = {0};  
    // gebruikt sectoren bijhouden (0 = vrij, 1 = bezet)

    uint8_t buffer[SECTOR_SIZE];

    // 1. Scan de file-table (heel kleine hoeveelheid disk reads)
    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;

        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[32];
            int len = 0;

            // naam lezen
            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 31)
                naam[len++] = buffer[i++];
            naam[len] = '\0';

            if (buffer[i++] != ':') break;

            // sector nummer lezen
            int sector = 0;
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9')
                sector = sector * 10 + (buffer[i++] - '0');

            // skip tot newline
            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;
            if (i < SECTOR_SIZE) i++;

            // geldige sector markeren
            if (sector > 0 && sector < total_sectors)
                used[sector] = 1;
        }
    }

    // 2. Zoek de eerste vrije sector (GEEN disk reads!)
    uint32_t start = file_table_start + file_table_sectors;
    for (uint32_t s = start; s < total_sectors; s++) {
        if (used[s] == 0) {
            return s;   // eerste vrije sector gevonden
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
// Een functie om te controleren of het opgegeven pad een geldige directory is.
int fs_is_directory(const char* pad) {
    uint8_t buffer[SECTOR_SIZE];
    
    // Zoek naar de directory in de bestandstabel (bijv. controleren of pad bestaat)
    for (uint32_t s = file_table_start; s < file_table_start + file_table_sectors; s++) {
        if (ata_read_sector(s, buffer) != 0) continue;  // Fout bij lezen van sector

        int i = 0;
        while (i < SECTOR_SIZE) {
            char naam[64];
            int len = 0;

            // Lees bestandsnaam
            while (i < SECTOR_SIZE && buffer[i] != ':' && len < 63) naam[len++] = buffer[i++];
            naam[len] = '\0';  // Eindig de naam

            if (buffer[i++] != ':') break;  // Verwacht een ":" na de naam

            int sector = 0;
            // Lees de sector
            while (i < SECTOR_SIZE && buffer[i] >= '0' && buffer[i] <= '9') {
                sector = sector * 10 + (buffer[i++] - '0');
            }

            while (i < SECTOR_SIZE && buffer[i] != '\n') i++;  // Sla de newline over

            // Vergelijk de directorynaam
            if (strcmp(naam, pad) == 0) {
                return 1;  // De directory bestaat
            }
        }
    }

    return 0;  // Directory niet gevonden
}
