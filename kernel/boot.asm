; ===================================================================
; BLUE OS - /kernel/boot.asm (Entry point)
; ===================================================================

global _start
extern kernel_main              ; Função principal no kernel.c

section .multiboot_header
align 8
header_start:
    dd 0xe85250d6               ; Multiboot2 Magic Number
    dd 0                        ; Modo Protegido i386
    dd header_end - header_start; Tamanho do cabeçalho
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; Checksum

    dw 0                        ; End Tag
    dw 0
    dd 8
header_end:

section .bss
align 16
stack_bottom:
    resb 16384                  ; 16 KB de Stack para o Kernel
stack_top:

section .text
bits 32
_start:
    mov esp, stack_top          ; Configura o ponteiro da pilha
    xor eax, eax
    xor ebx, ebx

    call kernel_main            ; Entrega o controle para o kernel.c

.halt:
    cli
    hlt
    jmp .halt
    
