cmake_minimum_required(VERSION 3.10)
project(BlueGameEngine C ASM_NASM)

# 1. Informa ao compilador para buscar cabeçalhos na pasta include/ e kernel/
include_directories(include kernel)

# 2. Definição do executável do Kernel
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/panic.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Se você preferir usar a sintaxe moderna por alvo (target), em vez de include_directories:
# target_include_directories(blue_game_engine PRIVATE include kernel)
