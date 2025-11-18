sudo qemu-system-i386 -cdrom build/mykernel.iso -serial stdio -audiodev alsa,id=snd0 -machine pcspk-audiodev=snd0 -drive file=disk.img,format=raw
