ENTRY(_start)

SECTIONS {
    . = 1M;

    .multiboot : { *(.multiboot) }

    .text :   { *(.text) }

    .rodata : { *(.rodata) }

    .data :   { *(.data) }

    .bss : {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }
}