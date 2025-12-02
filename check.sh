#!/bin/bash
set -e

echo "=== Kernel check ==="
if grub-file --is-x86-multiboot iso/boot/kernel.bin; then
    echo "[OK] kernel.bin heeft een geldige Multiboot-header"
else
    echo "[FOUT] kernel.bin is NIET multiboot-compatibel"
    exit 1
fi

echo "=== grub.cfg check ==="
if [ -f iso/boot/grub/grub.cfg ]; then
    echo "[OK] grub.cfg gevonden in iso/boot/grub/"
else
    echo "[FOUT] grub.cfg ontbreekt!"
    exit 1
fi

echo "=== ISO check ==="
if [ -f mykernel.iso ]; then
    if file mykernel.iso | grep -q "bootable"; then
        echo "[OK] mykernel.iso is bootable"
    else
        echo "[FOUT] mykernel.iso is NIET bootable"
        exit 1
    fi
else
    echo "[FOUT] mykernel.iso ontbreekt!"
    exit 1
fi

echo "=== USB check ==="
USB=/dev/sdb   # pas dit aan naar jouw stick
if lsblk | grep -q "$(basename $USB)"; then
    echo "[OK] USB-stick $USB gevonden"
    if sudo fdisk -l $USB | grep -q "FAT32"; then
        echo "[OK] USB-stick heeft een FAT32 partitie"
    else
        echo "[WAARSCHUWING] USB-stick is niet FAT32 geformatteerd"
    fi
else
    echo "[FOUT] USB-stick $USB niet gevonden"
    exit 1
fi

echo "=== Bootbestanden op USB check ==="
sudo mount ${USB}1 /mnt
if [ -f /mnt/boot/grub/grub.cfg ] && [ -f /mnt/boot/kernel.bin ]; then
    echo "[OK] kernel.bin en grub.cfg staan op de USB"
else
    echo "[FOUT] kernel.bin of grub.cfg ontbreekt op de USB"
    sudo umount /mnt
    exit 1
fi
sudo umount /mnt

echo "=== Alles lijkt in orde! ==="
