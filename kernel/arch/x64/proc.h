#ifndef __ARCH_X64_PROC_H__
#define __ARCH_X64_PROC_H__

#include "flux.h"
#include "structures.h"

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

    uint64_t rflags;

    // Contains paging info, used to load correct page structure
    uint64_t cr3;
};

static struct processor_regs default_regs_userspace = {
    .rip = 0,
    .rax = 0, .rbx = 0, .rcx = 0, .rdx = 0,
    .rsi = 0, .rdi = 0, 
    .rsp = 0, .rbp = 0, 
    .r8  = 0, .r9  = 0, .r10 = 0, .r11 = 0, 
    .r12 = 0, .r13 = 0, .r14 = 0, .r15 = 0, 
    
    .cs = INIT_SEGMENT_SELECTOR(3, 0, 3),
    .ds = INIT_SEGMENT_SELECTOR(3, 0, 4),
    .ss = INIT_SEGMENT_SELECTOR(3, 0, 4),

    .rflags = 0,
    .cr3 = 0,
};

static struct processor_regs default_regs_kernel = {
    .rip = 0,
    .rax = 0, .rbx = 0, .rcx = 0, .rdx = 0,
    .rsi = 0, .rdi = 0, 
    .rsp = 0, .rbp = 0, 
    .r8  = 0, .r9  = 0, .r10 = 0, .r11 = 0, 
    .r12 = 0, .r13 = 0, .r14 = 0, .r15 = 0, 
    
    .cs = INIT_SEGMENT_SELECTOR(0, 0, 3),
    .ds = INIT_SEGMENT_SELECTOR(0, 0, 4),
    .ss = INIT_SEGMENT_SELECTOR(0, 0, 4),

    .rflags = 0,
    .cr3 = 0,
};

void print_regs(struct processor_regs *);

int init_task_space_from_mem(pml4_entry_t **pml4, uint64_t pmem_offset, uint64_t n_blocks);

#endif /* __ARCH_X64_PROC_H__ */