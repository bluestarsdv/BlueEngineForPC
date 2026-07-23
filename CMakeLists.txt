cmake_minimum_required(VERSION 3.20)

# Define o formato de saída do NASM como elf32 antes de inicializar o projeto
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
set(CMAKE_ASM_NASM_FLAGS "-f elf32")

project(BlueGameEngine LANGUAGES C ASM_NASM)

include_directories(kernel)

# Flags do C para bare-metal em 32-bits (x86)
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# Executável do Kernel (boot.img)
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Força o Linker a conectar em i386
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
