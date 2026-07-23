#include <stdint.h>

#define USER_CONFIG_PATH "/data/config/user.cfg"

static int vfs_file_exists(const char* path) {
    (void)path; // Silencia o warning de variavel nao usada
    return 0;   // Retorna 0 até o VFS ler o disco
}

void kernel_main(void) {
    // Se o arquivo de usuario nao existe na raiz /
    if (!vfs_file_exists(USER_CONFIG_PATH)) {
        // Exemplo: O Kernel manda o ELF Loader executar o wizard da system.img
        // load_and_run_elf("/system/setupwizard.elf");
    }

    while (1) {
        __asm__ volatile("hlt");
    }
}
