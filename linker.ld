ENTRY(kernel_main)

SECTIONS {
  . = 0x00100000;

  .multiboot : {
    KEEP(*(.multiboot))
  }

  .text : {
    *(.text*)
  }

  .rodata : {
    *(.rodata*)
  }

  .data : {
    *(.data*)
  }

  .bss : {
    *(.bss*)
    *(COMMON)
  }
}
