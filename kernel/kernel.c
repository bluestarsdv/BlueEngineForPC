#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define USER_CONFIG_PATH "/data/config/user.cfg"

static void clear_screen(uint8_t bg_color) {
    volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        vga_buffer[i] = (uint16_t)' ' | ((uint16_t)bg_color << 8);
    }
}

static void print_text(int row, int col, const char* str, uint8_t color) {
    if (!str || row < 0 || row >= SCREEN_HEIGHT || col < 0 || col >= SCREEN_WIDTH) return;

    volatile uint16_t* vga_buffer = (volatile uint16_t*)VGA_ADDRESS;
    int index = (row * SCREEN_WIDTH) + col;
    int i = 0;

    while (str[i] != '\0' && (col + i) < SCREEN_WIDTH) {
        vga_buffer[index + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
        i++;
    }
}

static int vfs_file_exists(const char* path) {
    return 0;
}

void kernel_main(void) {
    clear_screen(0x07);

    // Se o arquivo não existe, salta direto via Assembly para a função do setupwizard.c
    if (!vfs_file_exists(USER_CONFIG_PATH)) {
        __asm__ volatile ("call run_setup_wizard");
    }

    // Tela normal do sistema após o término do wizard
    clear_screen(0x07);
    print_text(2, 2, "Blue OS - Sistema Carregado", 0x0B);
    print_text(4, 2, "Bem-vindo ao sistema!", 0x0E);

    while (1) {
        __asm__ volatile("hlt");
    }
}
