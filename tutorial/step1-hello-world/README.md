## Install requirements
```bash
sudo apt install build-essential grub-pc-bin xorriso qemu
```

### how the print function works 
```c
void print(const char* msg) {
    char* vga = (char*)0xB8000; // start adress of the vga screen
    const int width = 80;

    for (int i = 0; msg[i]; i++) { // prints the text character for character
        char c = msg[i];


        // VGA output
        if (c == '\n') {
            cursor_x = 0;
            cursor_y++;
            continue;
        }

        int pos = (cursor_y * width + cursor_x) * 2;
        vga[pos] = c; // ASCII token to write on screen
        vga[pos + 1] = 0x07; // Colorbit: 0x07:  gray text on black background

        cursor_x++;
        if (cursor_x >= width) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    update_cursor(cursor_x, cursor_y); // move cursor to his next position
}
```
if you want to set output on the vga screen you must now this principe every character that you print correspond with a Colorbit like 0x07 that means gray text on a black background

```c
vga[pos] = c;
vga[pos + 1] = 0x07;
```
means the position x and y coordinates on the screen is the character to print the next position is a colorbit and do you set the color
### The build script

```bash
gcc -m32 -ffreestanding -nostdlib -lgcc -c kernel.c -o kernel.o
```
this line compiles the c file to a output file he doesn't compile the stdlib and make a freestanding file of it

```bash
ld -m elf_i386 -T linker.ld -o kernel.bin kernel.o
```
links all output files to kernel.bin here one kernel.o
```bash
cat > iso/boot/grub/grub.cfg <<EOF
set timeout=5
set default=0

# Forceer tekstmodus zodat je shell zichtbaar is
set gfxpayload=text

menuentry "Your kernel" {
    multiboot /boot/kernel.bin
    boot
}

EOF

echo "[+] Building mykernel.iso..."
grub-mkrescue -o mykernel.iso iso
```
use grub to make from that kernel.bin an iso file