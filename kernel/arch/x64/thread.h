#ifndef __ARCH_X64_THREAD_H__
#define __ARCH_X64_THREAD_H__

#include "flux.h"

#include <stddef.h>
#include <stdint.h>

struct processor_regs {
    // General purpose register values
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // Instruction pointer
    uint64_t rip;

    // Segment registers
    uint16_t cs;
    uint16_t ds;
    uint16_t ss;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;

    uint64_t rflags;

    // Contains paging info, used to load correct page structure
    uint64_t cr3;
};

/*  Save the current CPU registers into memory. */
void save_regs(struct processor_regs *);

/*  Load CPU registers from memory into the CPU.
    Loading the instruction pointer will jump to where the task was last
    executing, and so this function will not return. */
void load_regs(struct processor_regs *);

void print_regs(struct processor_regs *);

#endif /* __ARCH_X64_THREAD_H__ */