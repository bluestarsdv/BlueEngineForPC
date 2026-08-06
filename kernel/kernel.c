#include <panic.h>

// Tipos manuais
typedef unsigned char  u8_t;
typedef unsigned short u16_t;
typedef unsigned int   u32_t;

// --- CONFIGURAÇÃO VESA / FRAMEBUFFER ---
// Nota: Em um SO completo, o endereço do framebuffer, largura e altura 
// devem vir da estrutura de boot (ex: Multiboot do GRUB).
#define FRAMEBUFFER_ADDR 0xFD000000 // Subsitua pelo endereço LFB do seu ambiente
#define SCREEN_WIDTH     1024
#define SCREEN_HEIGHT    768
#define SCREEN_PITCH     1024       // Pixels por linha

// Cores no formato 0xAARRGGBB (32-bit True Color)
#define COLOR_BG    0xFF0000AA  // Azul escuro
#define COLOR_TEXT  0xFFFFFFFF  // Branco

#define USER_CONFIG_PATH "/data/config/user.cfg"

// Ponteiro global para a memória de vídeo VESA
static volatile u32_t* framebuffer = (volatile u32_t*) FRAMEBUFFER_ADDR;

// Desenha um único pixel no modo VESA
static void draw_pixel(int x, int y, u32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    framebuffer[y * SCREEN_PITCH + x] = color;
}

// Limpa a tela preenchendo o framebuffer com a cor de fundo
static void clear_screen(u32_t color) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        framebuffer[i] = color;
    }
}

// Fonte bitmap 8x8 simples para desenhar texto no modo gráfico
// Cada byte representa uma linha de 8 pixels de um caractere
static const u8_t font8x8_basic[128][8] = {
    // Para simplificar, o array completo da fonte pode ser inserido aqui.
    // Exemplo do caractere 'A':
    ['A'] = {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},
    ['B'] = {0x3E, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3E, 0x00},
    // ...
};

// Desenha um caractere pixel a pixel
static void draw_char(int x, int y, char c, u32_t color) {
    if ((u8_t)c > 127) return;
    
    for (int row = 0; row < 8; row++) {
        u8_t row_pixels = font8x8_basic[(u8_t)c][row];
        for (int col = 0; col < 8; col++) {
            // Se o bit estiver ativo na fonte, desenha o pixel
            if (row_pixels & (1 << (7 - col))) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

// Desenha uma string de texto na tela de forma gráfica
static void print_text_gfx(int x, int y, const char *text, u32_t color) {
    int current_x = x;
    for (int i = 0; text[i] != '\0'; i++) {
        draw_char(current_x, y, text[i], color);
        current_x += 8; // Avança 8 pixels para o próximo caractere
    }
}

static int vfs_file_exists(const char* path) {
    (void)path; // Silencia o warning de variável não usada
    return 0;   // Retorna 0 até o VFS ler o disco
}

void kernel_main(void) {
    // 1. Limpa a tela com o fundo azul
    clear_screen(COLOR_BG);

    // 2. Desenha o banner da Blue Game Engine na tela em modo gráfico
    // As posições agora são em pixels (x, y) em vez de colunas/linhas de texto
    print_text_gfx(20, 20, "============================================================", COLOR_TEXT);
    print_text_gfx(20, 36, "                    BLUE GAME ENGINE                        ", COLOR_TEXT);
    print_text_gfx(20, 52, "============================================================", COLOR_TEXT);

    print_text_gfx(20, 84, "YouTube: https://youtube.com/@yeahblue2026", COLOR_TEXT);
    print_text_gfx(20, 100, "Site: https://bluegameengine.web.app/", COLOR_TEXT);

    print_text_gfx(20, 132, "------------------------------------------------------------", COLOR_TEXT);

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
