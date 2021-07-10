OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;
    
    _KernelStart = .;
    .text : ALIGN(0x1000)
    {
        *(.text)
    }

    /* Read-write data (initialized) */
    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    /* Read-only data. */
    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }

    /* Read-write data (uninitialized) and stack */
    .bss : ALIGN(0x1000)
	{
		*(COMMON)
		*(.bss)
	}
    
    _KernelEnd = .;
}