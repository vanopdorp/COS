#!/bin/bash
set -e

echo "[+] Compiling C source files..."
gcc -m32 -ffreestanding -nostdlib -lgcc -c kernel.c -o kernel.o
echo "[+] Linking kernel..."
ld -m elf_i386 -T linker.ld -o kernel.bin kernel.o

echo "[✓] Build complete: kernel.bin ready."

echo "[+] Preparing ISO folder..."
mkdir -p iso/boot/grub
cp kernel.bin iso/boot/kernel.bin

echo "[+] Creating grub.cfg..."
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

echo "[✓] ISO ready: mykernel.iso"
sudo qemu-system-i386 -cdrom mykernel.iso
