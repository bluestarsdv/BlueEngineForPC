typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define VGA_ADDRESS 0xB8000

void clear_screen(void) {
    volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;
    for (int i = 0; i < 80 * 25; ++i) {
        vga_buffer[i] = (uint16_t)' ' | (0x07 << 8);
    }
}

void print_secure(int row, int col, const char* str, uint8_t color) {
    if (str == (void*)0 || row < 0 || row >= 25 || col < 0 || col >= 80) return;

    volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;
    int index = (row * 80) + col;
    int i = 0;

    while (str[i] != '\0' && (col + i) < 80) {
        vga_buffer[index + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
        i++;
    }
}

void kernel_main(void) {
    clear_screen();

    print_secure(2, 2, "Blue Game Engine", 0x0B);                 // Ciano
    print_secure(3, 2, "By @yeahblue2026 in YouTube!", 0x0E);     // Amarelo
    print_secure(5, 2, "Copyright Yeah Blue, 2026, all rights reserved", 0x07); // Cinza

    // Kernel pronto para executar ELF / EXE / PAK
    while (1) {
        __asm__ volatile("hlt");
    }
}
