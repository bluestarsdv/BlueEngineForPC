cmake_minimum_required(VERSION 3.20)
project(BlueGameEngine LANGUAGES C ASM_NASM)

# Permite incluir cabeçalhos da pasta kernel/
include_directories(kernel)

# Flags do compilador para bare-metal (x86 32-bit)
set(CMAKE_C_FLAGS "-m32 -ffreestanding -O2 -Wall -Wextra -nostdlib")

# Flags do NASM para gerar objeto ELF 32-bit
set_property(SOURCE kernel/boot.asm PROPERTY COMPILE_FLAGS "-f elf32")

# Executável do Kernel/Engine com todos os arquivos do sistema
add_executable(blue_game_engine
    kernel/boot.asm
    kernel/kernel.c
    kernel/setupwizard.c
    kernel/elf_loader.c
    kernel/exe_loader.c
    kernel/config_reader.c
    kernel/process_manager.c
)

# Garante que o linker gere o binário sem bibliotecas padrão do C
set_target_properties(blue_game_engine PROPERTIES LINK_FLAGS "-m32 -nostdlib")
