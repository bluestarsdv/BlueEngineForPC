cmake_minimum_required(VERSION 3.20)
project(BlueGameEngine LANGUAGES C ASM_NASM)

include_directories(kernel)

# Flags do C para bare-metal em 32-bits (x86)
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# FORÇA O NASM A USAR ELF32 NESTE ARQUIVO ESPECÍFICO (Resolve o erro do ld)
set_source_files_properties(kernel/boot.asm PROPERTIES COMPILE_FLAGS "-f elf32")

# Executável do Kernel (boot.img)
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Força o linker a compilar para i386 (32-bit)
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
