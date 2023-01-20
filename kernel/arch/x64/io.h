#ifndef __ARCH_X64_IO_H__
#define __ARCH_X64_IO_H__

#include "flux.h"

#include <stdint.h>

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
    return ret;
}

inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1"
                    :
                    : "a"(val), "Nd"(port));
}

#endif