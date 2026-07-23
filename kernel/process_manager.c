/* ===================================================================
   BLUE OS - kernel/process_manager.c
   Gerenciador Padrão de Processos
   =================================================================== */

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define MAX_PROCESSES 16

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_TERMINATED
} ProcessState;

typedef struct {
    uint32_t pid;
    ProcessState state;
    uint32_t entry_point;
} Process;

static Process process_table[MAX_PROCESSES];
static uint32_t current_process_count = 0;

// Cria um novo processo no sistema
int create_process(uint32_t entry_point) {
    if (current_process_count >= MAX_PROCESSES) {
        return -1; // Tabela de processos cheia
    }

    process_table[current_process_count].pid = current_process_count + 1;
    process_table[current_process_count].state = PROCESS_READY;
    process_table[current_process_count].entry_point = entry_point;

    current_process_count++;
    return process_table[current_process_count - 1].pid;
}
