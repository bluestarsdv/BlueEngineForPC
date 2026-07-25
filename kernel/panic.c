#include <panic.h>

// Memória de vídeo VGA no modo texto (80x25)
#define VGA_MEMORY ((volatile u16_t*) 0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define COLOR_PANIC 0x4F00 // Texto Branco com Fundo Vermelho

static void clear_screen_panic(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = COLOR_PANIC | ' ';
    }
}

static void print_panic_text(int row, int col, const char *text) {
    int index = row * VGA_WIDTH + col;
    for (int i = 0; text[i] != '\0'; i++) {
        VGA_MEMORY[index + i] = COLOR_PANIC | text[i];
    }
}

void panic(const char *message) {
    // 1. Trava interrupções de hardware
    __asm__ volatile("cli");

    // 2. Limpa a tela com fundo vermelho
    clear_screen_panic();

    // 3. Renderiza mensagem
    print_panic_text(2, 2, "============================================================");
    print_panic_text(3, 2, "                    BLUE ENGINE - KERNEL PANIC              ");
    print_panic_text(4, 2, "============================================================");

    print_panic_text(6, 2, "A critical system error has occurred.");
    print_panic_text(8, 2, "Reason:");
    if (message) {
        print_panic_text(9, 4, message);
    } else {
        print_panic_text(9, 4, "Unknown Panic Reason");
    }

    print_panic_text(12, 2, "System Status: HALTED. Please restart the system.");

    // 4. Parada segura do processador
    while (1) {
        __asm__ volatile("hlt");
    }
}
