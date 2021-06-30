[bits 64]

LoadGDT:
    lgdt [rdi]      ; Load GDT table

    mov ax, 0x10    ; Select the Kernel Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi         ; Pop the return address in rdi
    mov rax, 0x08    ; Select the Kernel Code
    push rax
    push rdi
    retfq

GLOBAL LoadGDT