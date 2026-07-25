#ifndef KERNEL_PANIC_H
#define KERNEL_PANIC_H

// Tipos inteiros manuais para garantir autonomia bare-metal
typedef unsigned char      u8_t;
typedef unsigned short     u16_t;
typedef unsigned int       u32_t;
typedef unsigned int       uintptr_t;

// Função de Panic
void panic(const char *message);

#endif // KERNEL_PANIC_H
