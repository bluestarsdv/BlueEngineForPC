cmake_minimum_required(VERSION 3.20)
project(BlueGameEngine LANGUAGES C ASM_NASM)

# Permite incluir cabeçalhos do kernel e do sysroot
include_directories(kernel sysroot/system)

# Flags para compilação bare-metal em 32-bits
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# Configuração do NASM
set_property(SOURCE kernel/boot.asm PROPERTY COMPILE_FLAGS "-f elf32")

# Executável do sistema compilando os arquivos dos seus respectivos locais
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    sysroot/system/setupwizard.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Configuração do Linker
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
