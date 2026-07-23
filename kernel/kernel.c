/* ===================================================================
   BLUE OS KERNEL - kernel/kernel.c
   Ambiente Freestanding (Sem C Standard Library)
   =================================================================== */

// Definição nativa dos tipos de dados para Bare-Metal
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define VGA_ADDRESS 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Limpa a memória de vídeo com segurança
void clear_screen(void) {
    volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        vga_buffer[i] = (uint16_t)' ' | (0x07 << 8);
    }
}

// Impressão segura com checagem manual de memória e limites
void print_secure(int row, int col, const char* str, uint8_t color) {
    // 1. Proteção contra ponteiro NULO
    if (str == (void*)0) return;

    // 2. Proteção contra limites físicos da tela VGA
    if (row < 0 || row >= SCREEN_HEIGHT) return;
    if (col < 0 || col >= SCREEN_WIDTH) return;

    volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;
    int index = (row * SCREEN_WIDTH) + col;
    int i = 0;

    // 3. Trava de estouro de linha
    while (str[i] != '\0' && (col + i) < SCREEN_WIDTH) {
        vga_buffer[index + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
        i++;
    }
}

void kernel_main(void) {
    clear_screen();

    // Boot inicial e apresentação oficial
    print_secure(2, 2, "Blue Game Engine", 0x0B);                 // Ciano
    print_secure(3, 2, "By @yeahblue2026 in YouTube!", 0x0E);     // Amarelo
    print_secure(5, 2, "Copyright Yeah Blue, 2026, all rights reserved", 0x07); // Cinza

    // Loop infinito de baixo nível para manter o processador em espera
    while (1) {
        __asm__ volatile("hlt");
    }
}
