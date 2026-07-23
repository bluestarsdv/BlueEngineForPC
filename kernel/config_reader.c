/* ===================================================================
   BLUE OS - kernel/config_reader.c
   Leitor e parser nativo de arquivos .cfg para ambiente bare-metal
   =================================================================== */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// Função auxiliar para comparar duas strings simples
static int string_equals(const char* s1, const char* s2, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        if (s1[i] != s2[i]) return 0;
        if (s1[i] == '\0' || s2[i] == '\0') break;
    }
    return 1;
}

// Procura por uma chave (ex: "video_mode") dentro do texto do arquivo .cfg
int config_get_string(const char* file_buffer, const char* key, char* out_value, uint32_t max_out_len) {
    if (file_buffer == (void*)0 || key == (void*)0 || out_value == (void*)0) {
        return 0; // Proteção contra ponteiro nulo
    }

    uint32_t i = 0;
    uint32_t key_len = 0;

    // Calcula o tamanho da chave buscada
    while (key[key_len] != '\0') key_len++;

    while (file_buffer[i] != '\0') {
        // Ignora comentários (#) e linhas de seções ([boot])
        if (file_buffer[i] == '#' || file_buffer[i] == '[') {
            while (file_buffer[i] != '\n' && file_buffer[i] != '\r' && file_buffer[i] != '\0') {
                i++;
            }
            if (file_buffer[i] != '\0') i++;
            continue;
        }

        // Tenta casar a chave na linha atual
        if (string_equals(&file_buffer[i], key, key_len)) {
            // Verifica se o caractere seguinte é '=' (com ou sem espaços)
            uint32_t cursor = i + key_len;
            while (file_buffer[cursor] == ' ' || file_buffer[cursor] == '\t') cursor++;

            if (file_buffer[cursor] == '=') {
                cursor++; // Pula o '='
                while (file_buffer[cursor] == ' ' || file_buffer[cursor] == '\t') cursor++;

                // Copia o valor até o final da linha
                uint32_t val_idx = 0;
                while (file_buffer[cursor] != '\n' && 
                       file_buffer[cursor] != '\r' && 
                       file_buffer[cursor] != '\0' && 
                       val_idx < (max_out_len - 1)) {
                    out_value[val_idx++] = file_buffer[cursor++];
                }
                out_value[val_idx] = '\0'; // Finaliza a string
                return 1; // Sucesso!
            }
        }

        // Avança para a próxima linha
        while (file_buffer[i] != '\n' && file_buffer[i] != '\0') {
            i++;
        }
        if (file_buffer[i] != '\0') i++;
    }

    return 0; // Chave não encontrada
}

// Converte o valor lido da chave para um número inteiro (ex: 80x25 ou números de porta)
int config_get_int(const char* file_buffer, const char* key, int default_value) {
    char val_buffer[32];
    if (config_get_string(file_buffer, key, val_buffer, sizeof(val_buffer))) {
        int result = 0;
        int i = 0;
        while (val_buffer[i] >= '0' && val_buffer[i] <= '9') {
            result = result * 10 + (val_buffer[i] - '0');
            i++;
        }
        return (i > 0) ? result : default_value;
    }
    return default_value;
}
