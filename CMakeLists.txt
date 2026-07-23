cmake_minimum_required(VERSION 3.20)
project(BlueGameEngine LANGUAGES C ASM_NASM)

include_directories(kernel sysroot/system)

# Flags do C para bare-metal 32-bit
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# Força o NASM a gerar o arquivo objeto em 32-bit (elf32)
set(CMAKE_ASM_NASM_FLAGS "-f elf32")

add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    sysroot/system/setupwizard.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Força o linker a compilar em i386
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
