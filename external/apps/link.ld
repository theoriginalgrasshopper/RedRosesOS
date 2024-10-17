/* flat_linker.ld */
OUTPUT_FORMAT(binary)   /* Output raw binary format */
ENTRY(_start)           /* Define the entry point as _start */
SECTIONS
{
    /* Load the text (code) section at address 0 */
    . = 0x0;
    .text : {
        *(.text)
    }

    /* Load the data section */
    .data : {
        *(.data)
    }

    /* BSS section (uninitialized data) */
    .bss : {
        *(COMMON)
        *(.bss)
    }
}

