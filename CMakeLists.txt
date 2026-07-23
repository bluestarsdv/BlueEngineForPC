cmake_minimum_required(VERSION 3.20)
project(BlueGameEngine LANGUAGES C ASM_NASM)

include_directories(kernel)

# Flags do C para bare-metal em 32-bits
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# Força o NASM a gerar ELF 32-bit explicitamente no boot.asm
set_source_files_properties(kernel/boot.asm PROPERTIES COMPILE_FLAGS "-f elf32")

# Binário principal do KERNEL (vai para a boot.img)
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Força o linker a trabalhar em x86 (i386)
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
