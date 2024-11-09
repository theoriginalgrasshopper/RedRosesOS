OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)
SECTIONS
{
    . = 0x0;

    .text : {
        _start = .;
        *(.text)
    }

    .data : {
        *(.data)
    }

    .rodata : {
        *(.rodata)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}
