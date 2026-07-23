cmake_minimum_required(VERSION 3.20)
project(BlueGameEngine LANGUAGES C ASM_NASM)

include_directories(kernel)

# 1. Força o NASM a compilar SEMPRE em ELF 32-bit
set(CMAKE_ASM_NASM_FLAGS "-f elf32")

# 2. Flags do C para bare-metal em 32-bits (x86)
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# 3. Apenas os arquivos do KERNEL (vão para a boot.img)
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# 4. Força o Linker a conectar tudo em i386 (32-bit)
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
