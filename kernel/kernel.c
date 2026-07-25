#include <panic.h>

// Tipos manuais sem depender do stdint.h
typedef unsigned short u16_t;
typedef unsigned int   u32_t;

// Endereço da memória de vídeo VGA no modo texto (80x25)
#define VGA_MEMORY ((volatile u16_t*) 0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

// Cor: Fundo Azul (0x1000) + Texto Branco (0x0F00) -> 0x1F00
#define COLOR_THEME 0x1F00

#define USER_CONFIG_PATH "/data/config/user.cfg"

static void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = COLOR_THEME | ' ';
    }
}

static void print_text(int row, int col, const char *text) {
    int index = row * VGA_WIDTH + col;
    for (int i = 0; text[i] != '\0'; i++) {
        VGA_MEMORY[index + i] = COLOR_THEME | text[i];
    }
}

static int vfs_file_exists(const char* path) {
    (void)path; // Silencia o warning de variável não usada
    return 0;   // Retorna 0 até o VFS ler o disco
}

void kernel_main(void) {
    // 1. Limpa a tela
    clear_screen();

    // 2. Desenha o banner com os links da Blue Game Engine na tela
    print_text(2, 2, "============================================================");
    print_text(3, 2, "                    BLUE GAME ENGINE                        ");
    print_text(4, 2, "============================================================");

    print_text(6, 2, "YouTube: https://youtube.com/@yeahblue2026");
    print_text(7, 2, "Site: https://bluegameengine.web.app/");

    print_text(9, 2, "------------------------------------------------------------");

    // 3. Checagem do VFS
    if (!vfs_file_exists(USER_CONFIG_PATH)) {
        // Se não encontrou o arquivo, aciona o Kernel Panic!
        panic("User config missing! Executing fallback or setup wizard.");
    }

    // Loop de espera caso tudo ocorra bem
    while (1) {
        __asm__ volatile("hlt");
    }
}
