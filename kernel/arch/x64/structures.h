/* structures.h

This file contains C structs and types corresponding to all relevant structures
used by the x86-64 (ia-32e) architecture. Any system structures that are used
by the kernel, but not defined in the Intel Developer Manual, will not be in
this file, and instead likely in a header file related to their functionality.

Contributors: (add your name if you modify this file)

 - Jacob Garby <j4cobgarby@gmail.com>

*/

#ifndef __INCLUDE_X64_STRUCTURES_H__
#define __INCLUDE_X64_STRUCTURES_H__

#include "flux.h"
#include <stdint.h>

/* A segment_selector_t type represents a segment selector, which is used all
 * over the place when dealing with memory segmentation.
 * 
 * Bits 0-1 make up the requested privelage level. This is the "ring".
 *  0 = Kernel mode, 1,2 = Drivers, 3 = Userspace
 * The flux kernel uses only rings 0 and 3.
 *
 * Bit 2 specifies the descriptor table that the segment being referred to is
 *  in. 0 = GDT, 1 = LDT.
 *
 * Bits 3-15 contain the index of the entry in the specified table (GDT or LDT).
 */
typedef uint16_t segment_selector_t;
#define INIT_SEGMENT_SELECTOR(rpl, table_index, index) \
    ((rpl & 0x03) | ((table_index & 0x01) << 2) | (index << 3))
#define SEGMENT_SELECTOR_RPL(selector) ((selector) & 0x03)
#define SEGMENT_SELECTOR_TABLE_INDEX(selector) ((selector >> 2) & 0x01)
#define SEGMENT_SELECTOR_INDEX(selector) ((selector >> 3))

#define SEGMENT_SELECTOR_TABLE_GDT 0
#define SEGMENT_SELECTOR_TABLE_LDT 1

/* An idt_attributes_t type represents the attribute field within an IDT table
 * entry. It contains information about the type of entry this is, which rings
 * can invoke the interrupt, and information relating to the task state segment.
 *
 * Bits 0-2 are the IST value. This refers to an index into the interrupt stack
 * table, if used. These entries can be used to load stacks when switching to
 * and from interrupt routines. This can be set to 0 if unneeded.
 *
 * Bits 8-11 are the gate type. In "long" (64-bit) mode, which is what this
 * header file deals with, this can be either:
 *  0xe, for a 64-bit interrupt gate, or
 *  0xf, for a 64-bit trap gate.
 *
 * Bits 13-14 are a 2-bit value representing the minimum privilege level needed
 * to invoke this interrupt from software.
 *
 * Bit 15 is 1 if this IDT entry is present in the IDT, or 0 otherwise.
 */
typedef uint16_t idt_attributes_t;
#define INIT_IDT_ATTRIBUTES(dpl, type, ist) \
    (1<<15 | (ist & 0x07) | ((type & 0xf) << 8) | ((dpl & 0x03) << 13))
#define IDT_ATTRIBUTES_IST(attributes) ((attributes) & 0x07)
#define IDT_ATTRIBUTES_TYPE(attributes) ((attributes >> 8) & 0x0f)
#define IDT_ATTRIBUTES_DPL(attributes) ((attributes >> 13) & 0x03)
#define IDT_ATTRIBUTES_PRESENT(attributes) ((attributes >> 15) & 0x01)

#define IDT_ATTRIBUTES_TYPE_INTERRUPT 0xe
#define IDT_ATTRIBUTES_TYPE_TRAP      0xf

/* An idt_entry refers to one entry in the IDT table.
 */
struct idt_entry {
    uint16_t offset_a; // Bits 0-15 of the address of the ISR.
    segment_selector_t segment_selector; 
    idt_attributes_t attributes;
    uint16_t offset_b;
    uint32_t offset_c; // Bits 32-63 of the address of the ISR.
    uint32_t reserved; // Reserved according to intel's specs. Set to 0.
} __attribute__((packed));

#define INIT_IDT_ENTRY(seg, attr, offset64) ((struct idt_entry) { \
    .segment_selector = seg, \
    .attributes = attr, \
    .offset_a = (offset64 & 0xffff), \
    .offset_b = ((offset64 >> 16) & 0xffff), \
    .offset_c = ((offset64 >> 32) & 0xffffffff), \
})

/* The IDT table is a 4096 byte (1 4KB page) long table of 256 IDT entries.
 * When an interrupt (hardware or software) is invoked (for example a keypress
 * or an instruction like `INT 50`), or an exception (like a page fault) occurs
 * then the CPU will look up the entry in this table corresponding to the 
 * interrupt vector number. Vectors for exceptions can be found at:
 *  https://wiki.osdev.org/Exceptions.
 * Once the IDT entry is found, and is present, the CPU will jump to the
 * specified service routine and start executing there.
 */

#define IDT_N_ENTRIES 256
extern struct idt_entry idt_table[IDT_N_ENTRIES];

/* An IDT descriptor is a structure that's passed to the LIDT instruction, in
 * order to tell the CPU to start using the IDT which this descriptor refers
 * to.
 */
struct idt_descriptor {
    uint16_t idt_size; // Size (bytes) of the IDT, minus 1.
    void* offset; // The address of the IDT. Paging applies!
} __attribute__((packed));

#define INIT_IDT_DESCRIPTOR(idt_offset, n_entries) ((struct idt_descriptor) { \
    .offset = idt_offset, \
    .idt_size = (n_entries * sizeof(struct idt_entry)) - 1, \
})

/* 
 */
struct isr_frame {
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
} __attribute__((packed));

typedef uint64_t register_cr3;

typedef uint64_t pml4_entry_t;
typedef uint64_t pdpt_entry_t;
typedef uint64_t pdt_entry_t;
typedef uint64_t pt_entry_t;

#define PSE_PRESENT     1 << 0  // Set if this entry is present in the table.
#define PSE_WRITEABLE   1 << 1  // If unset, this region of memory cannot be written to.
#define PSE_USER_MODE   1 << 2  // If unset, usermode accesses are not allowed.
#define PSE_PWT         1 << 3  //
#define PSE_PCD         1 << 4  // 
#define PSE_ACCESSED    1 << 5  // Set if this entry has been used for translation.
#define PSE_PAGE_SIZE   1 << 7  // Valid for PDP table entry and PD table entry. 0 if this entry references another paging structure, or 1 if it maps a page.
#define PSE_RESTART     1 << 11 // Used for HLAT paging, otherwise ignored. If set, restart translation as regular paging.
#define PSE_NO_EXEC     1 << 63 // Used if EFER.NXE set. If this is set, execution of memory in the region controlled by this structure isn't allowed.
#define PSE_DIRTY       1 << 6  // Valid in an entry that maps a page. Set by the CPU if memory in this page has been modified.
#define PSE_PAT         1 << 7  // 
#define PSE_GLOBAL      1 << 8  // 

#define PSE_PTR(val) (val & 0x000ffffffffff000)

#define CR3_BASE_ADDR(cr3) (cr3 & 0x000ffffffffff000)
#define EXTRACT_PML4_INDEX(vaddr)   ((vaddr >> 39) & 0x1ff)
#define EXTRACT_PDPT_INDEX(vaddr)   ((vaddr >> 30) & 0x1ff)
#define EXTRACT_PDT_INDEX(vaddr)    ((vaddr >> 21) & 0x1ff)
#define EXTRACT_PT_INDEX(vaddr)     ((vaddr >> 12) & 0x1ff)
#define EXTRACT_OFFSET(vaddr)       (vaddr & 0xfff)

#endif /* __INCLUDE_X64_STRUCTURES_H__ */