#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define USER_CONFIG_PATH "/data/config/user.cfg"

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void wizard_print(int row, int col, const char* str, uint8_t color) {
    if (!str) return;
    volatile uint16_t* vga = (volatile uint16_t*)VGA_ADDRESS;
    int index = (row * SCREEN_WIDTH) + col;
    int i = 0;

    while (str[i] != '\0' && (col + i) < SCREEN_WIDTH) {
        vga[index + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
        i++;
    }
}

void wizard_clear(uint8_t bg_color) {
    volatile uint16_t* vga = (volatile uint16_t*)VGA_ADDRESS;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        vga[i] = (uint16_t)' ' | ((uint16_t)bg_color << 8);
    }
}

static const char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

void read_input_field(int row, int col, char* buffer, int max_len, uint8_t is_password) {
    int pos = 0;
    buffer[0] = '\0';

    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            if (!(scancode & 0x80)) {
                char ch = scancode_map[scancode];

                if (ch == '\n') {
                    break;
                } else if (ch == '\b' && pos > 0) {
                    pos--;
                    buffer[pos] = '\0';
                    wizard_print(row, col + pos, " ", 0x1F);
                } else if (ch >= 32 && ch <= 126 && pos < max_len - 1) {
                    buffer[pos] = ch;
                    pos++;
                    buffer[pos] = '\0';

                    if (is_password) {
                        wizard_print(row, col + pos - 1, "*", 0x1E);
                    } else {
                        char temp[2] = {ch, '\0'};
                        wizard_print(row, col + pos - 1, temp, 0x1F);
                    }
                }
            }
        }
    }
}

void save_user_config(const char* username, const char* nickname, const char* password) {
    char buffer[256];
    int len = 0;

    const char* header = "[user]\nusername=";
    while (*header) buffer[len++] = *header++;
    while (*username) buffer[len++] = *username++;

    const char* nick_key = "\nnickname=";
    while (*nick_key) buffer[len++] = *nick_key++;
    while (*nickname) buffer[len++] = *nickname++;

    const char* pass_key = "\npassword=";
    while (*pass_key) buffer[len++] = *pass_key++;
    while (*password) buffer[len++] = *password++;

    const char* footer = "\nfirst_boot_completed=true\n";
    while (*footer) buffer[len++] = *footer++;
    buffer[len] = '\0';

    // vfs_write_file(USER_CONFIG_PATH, buffer, len);
}

void wait_for_finish_button(void) {
    wizard_print(14, 27, " [  TERMINEI!  ] ", 0xE0);
    wizard_print(16, 17, "Pressione ENTER no botao para confirmar", 0x1F);

    while (1) {
        if (inb(0x64) & 1) {
            uint8_t scancode = inb(0x60);
            if (!(scancode & 0x80)) {
                if (scancode_map[scancode] == '\n') break;
            }
        }
    }
}

void run_setup_wizard(void) {
    char username[32] = {0};
    char nickname[32] = {0};
    char password[32] = {0};

    wizard_clear(0x10);

    wizard_print(1, 25, "=== BLUE OS SETUP WIZARD ===", 0x1E);
    wizard_print(3, 5, "Configuracao Inicial do Sistema:", 0x1F);

    wizard_print(6, 5, "Nome de Usuario: ", 0x1F);
    read_input_field(6, 22, username, 30, 0);

    wizard_print(8, 5, "Apelido (Nickname): ", 0x1F);
    read_input_field(8, 25, nickname, 30, 0);

    wizard_print(10, 5, "Senha de Acesso: ", 0x1F);
    read_input_field(10, 22, password, 30, 1);

    wait_for_finish_button();

    save_user_config(username, nickname, password);

    wizard_clear(0x10);
    wizard_print(8, 22, "Conta criada com sucesso!", 0x1A);
    wizard_print(10, 20, "Carregando o Blue OS...", 0x1E);
}
