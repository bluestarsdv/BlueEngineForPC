/* ===================================================================
   BLUE OS - kernel/elf_loader.c
   Carregador e Executador NATIVO de Binários ELF (32-bit Bare-Metal)
   =================================================================== */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// Magic Number padrão de arquivos ELF: 0x7F 'E' 'L' 'F'
#define ELF_MAGIC0 0x7F
#define ELF_MAGIC1 'E'
#define ELF_MAGIC2 'L'
#define ELF_MAGIC3 'F'

// Tipo de segmento que deve ser carregado na RAM
#define PT_LOAD 1

// 1. Cabeçalho Principal do Arquivo ELF (32 bits)
typedef struct {
    uint8_t  e_ident[16];   // Assinatura e identificadores do formato
    uint16_t e_type;        // Tipo do arquivo (Executável, Relocável, etc)
    uint16_t e_machine;     // Arquitetura (Ex: x86)
    uint32_t e_version;     // Versão do formato ELF
    uint32_t e_entry;       // <--- PONTO DE ENTRADA (Endereço da primeira instrução)
    uint32_t e_phoff;       // Offset da Tabela de Cabeçalhos do Programa (Program Headers)
    uint32_t e_shoff;       // Offset da Tabela de Seções
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;   // Tamanho de cada entrada na tabela Program Header
    uint16_t e_phnum;       // Quantidade de entradas na tabela Program Header
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Ehdr;

// 2. Cabeçalho do Programa (Define seções que vão para a RAM)
typedef struct {
    uint32_t p_type;   // Tipo do segmento (Se é PT_LOAD)
    uint32_t p_offset; // Posição dos dados dentro do arquivo
    uint32_t p_vaddr;  // Endereço de Memória Virtual de destino na RAM
    uint32_t p_paddr;  // Endereço Físico de destino
    uint32_t p_filesz; // Tamanho dos dados no arquivo
    uint32_t p_memsz;  // Tamanho do segmento alocado na memória RAM
    uint32_t p_flags;  // Permissões (Read, Write, Execute)
    uint32_t p_align;
} Elf32_Phdr;

// Valida a assinatura de 4 bytes do arquivo ELF
int verify_elf_magic(const uint8_t* buffer) {
    if (buffer == (void*)0) return 0;

    if (buffer[0] == ELF_MAGIC0 &&
        buffer[1] == ELF_MAGIC1 &&
        buffer[2] == ELF_MAGIC2 &&
        buffer[3] == ELF_MAGIC3) {
        return 1; // É um binário ELF válido!
    }

    return 0; // Arquivo corrompido ou formato inválido
}

// Carrega o binário ELF e salta o ponteiro de instrução da CPU para ele
int load_and_run_elf(const uint8_t* elf_data) {
    // 1. Checa ponteiro nulo e assinatura mágica
    if (!verify_elf_magic(elf_data)) {
        return -1; // Falha na validação do ELF
    }

    Elf32_Ehdr* header = (Elf32_Ehdr*)elf_data;

    // 2. Mapeia as seções do binário para a memória RAM (Segmentos PT_LOAD)
    Elf32_Phdr* phdr = (Elf32_Phdr*)(elf_data + header->e_phoff);

    for (uint16_t i = 0; i < header->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint8_t* dest = (uint8_t*)phdr[i].p_vaddr;
            const uint8_t* src = elf_data + phdr[i].p_offset;

            // Copia as instruções e variáveis para o endereço real de RAM
            for (uint32_t b = 0; b < phdr[i].p_filesz; b++) {
                dest[b] = src[b];
            }

            // Zera o restante da memória alocada (Seções .bss de variáveis zeradas)
            for (uint32_t b = phdr[i].p_filesz; b < phdr[i].p_memsz; b++) {
                dest[b] = 0;
            }
        }
    }

    // 3. Obtém o ponteiro de função para o Ponto de Entrada (e_entry)
    typedef void (*entry_func_t)(void);
    entry_func_t entry_point = (entry_func_t)header->e_entry;

    // 4. Salto final da CPU: entrega o controle para o programa ELF executável!
    entry_point();

    return 0;
}
