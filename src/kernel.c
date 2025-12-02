
// Multiboot header
__attribute__((section(".multiboot"), used))
unsigned int multiboot_header[] = {
    0x1BADB002,         // magic
    0x00000003,         // flags
    0xE4524FFB          // checksum = -(magic + flags)
};
#include "time.h"
#include "pic.h"
#include "print.h"
#include "input.h"
#include "idt.h"
#include "string.h"
#include <stddef.h>
#include "heap.h"
#include "fmath.h"
#include "sound.h"
#include "fs.h"
#include "snprintf.h"
#include <stdbool.h>

void enable_fpu() {
    // Enable FPU: Clear Task Switched (TS) bit in CR0
    __asm__ __volatile__ (
        "mov %%cr0, %%eax\n"
        "and $0xFFFFFFFB, %%eax\n"  // Clear TS-bit (bit 3)
        "mov %%eax, %%cr0\n"
        "fninit\n"                  // Initialize FPU
        :
        :
        : "eax"
    );
}


#define MAX_ARGS 30
#define MAX_ARG_LEN 128
void set_timer_frequency(uint32_t hz);
// Forward declarations
void reboot_system();
void play_sound(int frequency);
void stop_sound();

typedef struct {
    char command[MAX_ARG_LEN];
    char args[MAX_ARGS][MAX_ARG_LEN];
    int arg_count;
} ParsedCommand;

void parse_command(char* input, ParsedCommand* result) {
    int i = 0, j = 0, k = 0;
    int in_arg = 0;

    // Reset result
    result->arg_count = 0;
    for (int m = 0; m < MAX_ARGS; m++) {
        result->args[m][0] = '\0';
    }

    // Skip leading spaces
    while (input[i] == ' ') i++;

    // Extract command
    while (input[i] != ' ' && input[i] != '\0') {
        result->command[j++] = input[i++];
    }
    result->command[j] = '\0';

    // Extract arguments
    while (input[i] != '\0') {
        if (input[i] == ' ') {
            if (in_arg) {
                result->args[result->arg_count][k] = '\0';
                result->arg_count++;
                k = 0;
                in_arg = 0;
            }
        } else {
            result->args[result->arg_count][k++] = input[i];
            in_arg = 1;
        }
        i++;
    }

    // Final argument
    if (in_arg) {
        result->args[result->arg_count][k] = '\0';
        result->arg_count++;
    }
}

int atoi(char* str) {
    int result = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return result;
}

// Exceptions
extern void isr0();  extern void isr1();  extern void isr2();  extern void isr3();
extern void isr4();  extern void isr5();  extern void isr6();  extern void isr7();
extern void isr8();  extern void isr9();  extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();

extern void irq0_stub();

void idt_init() {
    // Exceptions 0..31
    set_idt_gate(0,  (uint32_t)isr0);
    set_idt_gate(1,  (uint32_t)isr1);
    set_idt_gate(2,  (uint32_t)isr2);
    set_idt_gate(3,  (uint32_t)isr3);
    set_idt_gate(4,  (uint32_t)isr4);
    set_idt_gate(5,  (uint32_t)isr5);
    set_idt_gate(6,  (uint32_t)isr6);
    set_idt_gate(7,  (uint32_t)isr7);
    set_idt_gate(8,  (uint32_t)isr8);
    set_idt_gate(9,  (uint32_t)isr9);
    set_idt_gate(10, (uint32_t)isr10);
    set_idt_gate(11, (uint32_t)isr11);
    set_idt_gate(12, (uint32_t)isr12);
    set_idt_gate(13, (uint32_t)isr13);
    set_idt_gate(14, (uint32_t)isr14);
    set_idt_gate(15, (uint32_t)isr15);
    set_idt_gate(16, (uint32_t)isr16);
    set_idt_gate(17, (uint32_t)isr17);
    set_idt_gate(18, (uint32_t)isr18);
    set_idt_gate(19, (uint32_t)isr19);
    set_idt_gate(20, (uint32_t)isr20);
    set_idt_gate(21, (uint32_t)isr21);
    set_idt_gate(22, (uint32_t)isr22);
    set_idt_gate(23, (uint32_t)isr23);
    set_idt_gate(24, (uint32_t)isr24);
    set_idt_gate(25, (uint32_t)isr25);
    set_idt_gate(26, (uint32_t)isr26);
    set_idt_gate(27, (uint32_t)isr27);
    set_idt_gate(28, (uint32_t)isr28);
    set_idt_gate(29, (uint32_t)isr29);
    set_idt_gate(30, (uint32_t)isr30);
    set_idt_gate(31, (uint32_t)isr31);


    // IRQ0 → 32
    set_idt_gate(32, (uint32_t)irq0_stub);

    load_idt();
}


#define MAX_LINES 100
#define MAX_LINE_LENGTH 256
#define MAX_TEXT_SIZE 1024 

// Get a specific line from text
char* get_line(char *text, int line_num) {
    static char line[MAX_LINE_LENGTH];
    int current = 0;
    char *start = text;
    char *end;

    while (current < line_num && start) {
        end = strchr(start, '\n');
        if (!end) break;
        start = end + 1;
        current++;
    }

    if (current == line_num) {
        end = strchr(start, '\n');
        if (!end) end = start + strlen(start);
        int len = end - start;
        if (len >= MAX_LINE_LENGTH) len = MAX_LINE_LENGTH - 1;
        strncpy(line, start, len);
        line[len] = '\0';
        return line;
    }

    return NULL;
}

void set_line(char* text, int text_size, int line_num, const char* new_line) {
    if (text == NULL || new_line == NULL || line_num < 0 || text_size <= 0) return;

    char tmp[MAX_TEXT_SIZE];
    int tpos = 0;
    tmp[0] = '\0';

    const char* s = text;
    int current = 0;
    int replaced = 0;

    /* Fast path: original empty */
    if (s[0] == '\0') {
        /* add blank lines until line_num (no leading newline for line 0) */
        for (; current < line_num && tpos < MAX_TEXT_SIZE - 1; ++current) tmp[tpos++] = '\n';
        /* append new line */
        for (const char* q = new_line; *q && tpos < MAX_TEXT_SIZE - 1; ++q) tmp[tpos++] = *q;
        tmp[tpos] = '\0';
        /* copy back respecting text_size */
        int copy_len = (tpos < text_size - 1) ? tpos : text_size - 1;
        if (copy_len > 0) memcpy(text, tmp, copy_len);
        text[copy_len] = '\0';
        return;
    }

    while (*s && tpos < MAX_TEXT_SIZE - 1) {
        const char* nl = strchr(s, '\n');
        int len = nl ? (int)(nl - s) : (int)strlen(s);

        if (current == line_num) {
            /* replace this line with new_line */
            for (const char* q = new_line; *q && tpos < MAX_TEXT_SIZE - 1; ++q) tmp[tpos++] = *q;
            replaced = true;
        } else {
            /* copy existing line */
            for (int i = 0; i < len && tpos < MAX_TEXT_SIZE - 1; ++i) tmp[tpos++] = s[i];
        }

        /* if original had a newline after this line, preserve it */
        if (nl) {
            if (tpos < MAX_TEXT_SIZE - 1) tmp[tpos++] = '\n';
            s = nl + 1;
        } else {
            s += len;
        }
        ++current;
    }

    /* If we did not replace (line_num beyond end), append as needed */
    if (!replaced) {
        /* if there is existing content and it doesn't end with a newline, add one to separate */
        if (tpos > 0 && tmp[tpos-1] != '\n' && current > 0 && tpos < MAX_TEXT_SIZE - 1) tmp[tpos++] = '\n';

        /* add empty lines until we reach the desired line */
        while (current < line_num && tpos < MAX_TEXT_SIZE - 1) {
            tmp[tpos++] = '\n';
            ++current;
        }

        /* append the new line */
        for (const char* q = new_line; *q && tpos < MAX_TEXT_SIZE - 1; ++q) tmp[tpos++] = *q;
    }

    tmp[tpos] = '\0';
    /* copy back respecting destination size */
    int copy_len = (tpos < text_size - 1) ? tpos : text_size - 1;
    if (copy_len > 0) memcpy(text, tmp, copy_len);
    text[copy_len] = '\0';
}

void init_filesystem() {
    fs_init();
    print("Filesystem initialized.\n");
}

// Simple password validation function
int validate_password(const char* input, const char* stored) {
    int i = 0;
    while (input[i] != '\0' && stored[i] != '\0') {
        if (input[i] != stored[i]) {
            return 0;
        }
        i++;
    }
    return (input[i] == '\0' && stored[i] == '\0');
}

// Login system
void login_system() {
    int login_attempts = 0;
    int max_attempts = 3;
    
    // Default credentials
    const char* default_username = "admin";
    const char* default_password = "password";
    
    print_color("\n=== Welcome to Kernel OS ===\n", 0x0A);
    print_color("Please login to continue\n\n", 0x0F);
    
    while (login_attempts < max_attempts) {
        print_color("Username: ", 0x0F);
        char username[128];
        strcpy(readStr(), username);
        
        print_color("Password: ", 0x0F);
        char password[128];
        strcpy(readStr(), password);
        
        if (validate_password(username, default_username) && 
            validate_password(password, default_password)) {
            print_color("\n", 0x0F);
            print_color("Login successful! Welcome, ", 0x0A);
            print_color(username, 0x0A);
            print_color("\n", 0x0A);
            return;
        } else {
            login_attempts++;
            if (login_attempts < max_attempts) {
                print_color("Login failed! Attempts remaining: ", 0x04);
                int remaining = max_attempts - login_attempts;
                char remaining_str[10];
                int_to_string(remaining, remaining_str);
                print_color(remaining_str, 0x04);
                print_color("\n\n", 0x04);
            }
        }
    }
    
    // Max attempts reached
    print_color("\nMaximum login attempts exceeded. System locked.\n", 0x04);
    print_color("Rebooting in 5 seconds...\n", 0x04);
    sleep_ms(5000);
    reboot_system();
}

void boot() {
    clear_screen();
    remap_pic();
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    idt_init();
    init_timer(100); // 100 Hz → ~10ms per tick

    // Enable only IRQ0 (timer)
    outb(0x21, 0xFE); // 11111110b → IRQ0 enabled
    outb(0xA1, 0xFF);

    // Enable interrupts
    __asm__ __volatile__("sti");
    heap_init();
    enable_fpu();
    play_sound(500);
    sleep_ms(300);
    stop_sound();

    init_filesystem();
    login_system();
}

void reboot_system() {
    print("Rebooting...\n");

    // Reset PIC
    outb(0x20, 0x11);  // Initialization phase of PIC1
    outb(0xA0, 0x11);  // Initialization phase of PIC2
    outb(0x21, 0x20);  // Offset for PIC1
    outb(0xA1, 0x28);  // Offset for PIC2
    outb(0x21, 0x04);  // Connect PIC1 with PIC2
    outb(0xA1, 0x02);  // Connect PIC2 with PIC1
    outb(0x21, 0x01);  // Set interrupt-enable bits
    outb(0xA1, 0x01);  // Set interrupt-enable bits for PIC2
    outb(0x20, 0x0);   // Restore PIC1
    outb(0xA0, 0x0);   // Restore PIC2

    // Reload the IDT
    load_idt();

    // Restart the system clock and other hardware like the timer
    init_timer(100);
    
    // Reset the machine via port 0x64
    outb(0x64, 0xFE);

    // System is now rebooted, stay in infinite loop
    while (1);
}



// Entry point
void help() {
    print("Available commands:\n");
    print("help       - show this help message\n");
    print("sethz      - set internal clock speed\n");
    print("sleep      - sleep in milliseconds\n");
    print("reboot     - reboot the system\n");
    print("logout     - logout and return to login screen\n");
    print("whoami     - show current username\n");
    print("clear      - clear the screen\n");
    print("beep       - make a beep sound\n");
    print("cat <file> - read a file\n");
    print("ls <path>  - list directory contents\n");
    print("cd <path>  - change directory\n");
    print("write <file> <content> - create a file with content\n");
}

void kernel_main() {
    boot();

    static char current_directory[128] = "/";
    static char logged_in_user[128] = "admin";
    int seconds = 0;

    while (1) {
        print("\n");
        print(logged_in_user);
        print("@COS:");
        print(current_directory);
        print("$ ");
        char* input = readStr();

        ParsedCommand cmd;
        parse_command(input, &cmd);

        if (cmd.arg_count > MAX_ARGS || strlen(input) > MAX_ARG_LEN * MAX_ARGS) {
            print_color("Input too long or too many arguments\n", 0x04);
            continue;
        }

        // help
        if (strcmp(cmd.command, "help") == 1) {
            help();
        }

        // clear
        else if (strcmp(cmd.command, "clear") == 1) {
            clear_screen();
        }

        // beep
        else if (strcmp(cmd.command, "beep") == 1) {
            play_sound(500);
            sleep_ms(300);
            stop_sound();
        }

        // sleep
        else if (strcmp(cmd.command, "sleep") == 1) {
            if (cmd.arg_count < 1) {
                print_color("Usage: sleep <milliseconds>\n", 0x04);
            } else {
                int milliseconds = atoi(cmd.args[0]);
                if (milliseconds <= 0) {
                    print_color("Invalid value for sleep\n", 0x04);
                } else {
                    sleep_ms((uint32_t)milliseconds);
                }
            }
        }

        // sethz
        else if (strcmp(cmd.command, "sethz") == 1) {
            if (cmd.arg_count < 1) {
                print_color("Usage: sethz <frequency in Hz>\n", 0x04);
            } else {
                int hz = atoi(cmd.args[0]);
                if (hz <= 0 || hz > 1000) {
                    print_color("Invalid frequency (1-1000 Hz allowed)\n", 0x04);
                } else {
                    init_timer((uint32_t)hz);
                    print("Timer frequency set to ");
                    print(cmd.args[0]);
                    print(" Hz\n");
                }
            }
        }

        // ls - list directory
        else if (strcmp(cmd.command, "ls") == 1) {
            const char* target = (cmd.arg_count < 1) ? current_directory : cmd.args[0];
            fs_list_directory(target);
        }

        // cd - change directory
        else if (strcmp(cmd.command, "cd") == 1) {
            if (cmd.arg_count < 1) {
                print_color("Usage: cd <path>\n", 0x04);
            } else {
                const char* argument = cmd.args[0];
                char full_path[256];
                full_path[0] = '\0';

                if (argument[0] == '/') {
                    strncpy(full_path, "/", sizeof(full_path));
                    strncat(full_path, argument + 1, sizeof(full_path) - strlen(full_path) - 1); 
                } else {
                    if (strcmp(current_directory, "/") == 1) {
                        snprintf(full_path, sizeof(full_path), "/%s", argument);
                    } else {
                        snprintf(full_path, sizeof(full_path), "%s/%s", current_directory, argument);
                    }
                }

                if (fs_is_directory(full_path)) {
                    strncpy(current_directory, full_path, sizeof(current_directory) - 1);
                    current_directory[sizeof(current_directory) - 1] = '\0';  
                    print("Current directory: ");
                    print(current_directory);
                    print("\n");
                } else {
                    print_color("Directory not found: ", 0x04);
                    print(full_path);
                    print("\n");
                }
            }
        }

        // cat - read file
        else if (strcmp(cmd.command, "cat") == 1) {
            if (cmd.arg_count < 1) {
                print_color("Usage: cat <filename>\n", 0x04);
            } else {
                char full_path[256];
                full_path[0] = '\0';
                if (strcmp(current_directory, "/") == 1) {
                    strcat(full_path, "/");
                } else {
                    strcat(full_path, current_directory);
                    strcat(full_path, "/");
                }
                strcat(full_path, cmd.args[0]);

                print(fs_read_file(full_path));
                print("\n");
            }
        }

        // reboot
        else if (strcmp(cmd.command, "reboot") == 1) {
            reboot_system();
        }
        else if (strcmp(cmd.command, "write") == 1) {
            char full_path[256];
            full_path[0] = '\0';
            if (strcmp(current_directory, "/") == 1) {
                strcat(full_path, "/");
            } else {
                strcat(full_path, current_directory);
                strcat(full_path, "/");
            }
            strcat(full_path, cmd.args[0]);

            fs_create_file(full_path, cmd.args[1],get_free_sector());
        }

        // whoami - show current user
        else if (strcmp(cmd.command, "whoami") == 1) {
            print_color("Current user: ", 0x0F);
            print_color(logged_in_user, 0x0A);
            print_color("\n", 0x0F);
        }

        // logout - return to login screen
        else if (strcmp(cmd.command, "logout") == 1) {
            print_color("Logging out...\n", 0x0A);
            sleep_ms(1000);
            clear_screen();
            login_system();
        }

        // unknown command
        else {
            print_color("Unknown command\n", 0x04);
        }
    }
}
