#ifndef __INCLUDE_X64_IDT_H__
#define __INCLUDE_X64_IDT_H__

#include "flux.h"

#include "structures.h"

void idt_init();
void idt_load(struct idt_entry* table, int n_entries);

#define INTERRUPT_HANDLER __attribute__((interrupt)) void

void idt_attach_handler(int vector, segment_selector_t seg, idt_attributes_t attr, void *handler);

/* Here we declare functions for many of the possible exceptions that can occur, as
 * well as other interrupts. Documentation about exceptions can be found in Vol. 3A,
 * Section 6 of the Intel Developer Manual, or at https://wiki.osdev.org/Exceptions.
 */
INTERRUPT_HANDLER isr_DE(struct isr_frame *frame);  // 0x00 Division Error
INTERRUPT_HANDLER isr_NMI(struct isr_frame *frame); // 0x02 Non-Maskable Interrupt
INTERRUPT_HANDLER isr_BP(struct isr_frame *frame);  // 0x03 Breakpoint
INTERRUPT_HANDLER isr_OF(struct isr_frame *frame);  // 0x04 Overflow
INTERRUPT_HANDLER isr_BR(struct isr_frame *frame);  // 0x05 Bound Range Exceeded
INTERRUPT_HANDLER isr_UD(struct isr_frame *frame);  // 0x06 Invalid Opcode
INTERRUPT_HANDLER isr_DF(struct isr_frame *frame, uint64_t error_code); // 0x08 Double Fault
INTERRUPT_HANDLER isr_TS(struct isr_frame *frame, uint64_t error_code); // 0x0a Invalid TSS
INTERRUPT_HANDLER isr_NP(struct isr_frame *frame, uint64_t error_code); // 0x0b Segment Not Present
INTERRUPT_HANDLER isr_SS(struct isr_frame *frame, uint64_t error_code); // 0x0c Stack Segment Fault
INTERRUPT_HANDLER isr_GP(struct isr_frame *frame, uint64_t error_code); // 0x0d General Protection Fault
INTERRUPT_HANDLER isr_PF(struct isr_frame *frame, uint64_t error_code); // 0x0e Page Fault

INTERRUPT_HANDLER isr_SYSCALL(struct isr_frame *frame); // 0x32 Syscall

#endif