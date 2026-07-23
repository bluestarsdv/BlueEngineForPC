/* ===================================================================
   BLUE OS - sysroot/system/setupwizard.c
   =================================================================== */

// Definição dos caminhos absolutos do Blue OS (Relativos à raiz '/')
#define USER_CONFIG_PATH "/data/config/user.cfg"

// Função responsável por salvar os dados quando o botão "Terminei!" é acionado
void save_user_config(const char* username, const char* nickname, const char* password) {
    /* Aqui o driver de sistema de arquivos do Kernel recebe o caminho '/data/config/user.cfg'
       e cria/escreve a estrutura:

       [user]
       username=<username>
       nickname=<nickname>
       password=<password>
       first_boot_completed=true
    */
    
    // Chamada do Kernel VFS (Virtual File System)
    // Example: vfs_write_file(USER_CONFIG_PATH, buffer, length);
}
