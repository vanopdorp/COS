#!/bin/bash
set -e

echo "[+] Compiling C source files..."
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/kernel.c -o build/kernel.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/print.c -o build/print.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/string.c -o build/string.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/ports.c -o build/ports.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/time.c -o build/time.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/idt.c -o build/idt.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/pic.c -o build/pic.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/exceptions.c -o build/exceptions.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/sound.c -o build/sound.o
gcc -m32 -ffreestanding -nostdlib -lgcc -c src/heap.c -o build/heap.o

echo "[+] Compiling assembly files..."
nasm -f elf32 src/idt_flush.asm -o build/idt_flush.o
nasm -f elf32 src/isr_default.asm -o build/isr_default.o
nasm -f elf32 src/irq0.asm -o build/irq0.o
nasm -f elf32 src/isr_exceptions.asm -o build/isr_exceptions.o

echo "[+] Linking kernel..."
ld -m elf_i386 -T linker.ld -o build/kernel.bin \
  build/kernel.o build/print.o build/string.o build/ports.o build/time.o build/idt.o \
  build/pic.o build/idt_flush.o build/isr_default.o build/irq0.o build/isr_exceptions.o build/exceptions.o build/sound.o \
  build/heap.o 

echo "[✓] Build complete: kernel.bin ready."

echo "[+] Preparing ISO folder..."
mkdir -p iso/boot/grub
cp build/kernel.bin iso/boot/kernel.bin

echo "[+] Creating grub.cfg..."
cat > iso/boot/grub/grub.cfg <<EOF
set timeout=5
set default=0

# Forceer tekstmodus zodat je shell zichtbaar is
set gfxpayload=text

menuentry "My Kernel" {
    multiboot /boot/kernel.bin
    boot
}

EOF

echo "[+] Building mykernel.iso..."
grub-mkrescue -o build/mykernel.iso iso

echo "[✓] ISO ready: mykernel.iso"

# Schrijf naar USB-stick 
dd if=mykernel.iso of=/dev/sdb bs=4M status=progress && sync
#run with:  sudo qemu-system-i386 -cdrom mykernel.iso -serial stdio -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 -drive file=disk.img,format=raw
dd if=/dev/zero of=disk.img bs=2048 count=28800