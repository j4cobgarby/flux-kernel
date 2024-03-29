/* mem.h

This file deals with memory management at the lowest level. That is, segmentation,
and any functions that deal only with physical memory.

Contributors: (add your name if you modify this file)

 - Jacob Garby <j4cobgarby@gmail.com>

*/

#ifndef __ARCH_X64_MEM_H__
#define __ARCH_X64_MEM_H__

#include "flux.h"

#include <stdint.h>
#include "structures.h"
#include "limine.h"

extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hddm_request;
extern volatile struct limine_kernel_address_request kernel_address_request;

/*  This structure will be present at the beginning of each page frame in memory
    so that, starting from the (logically) first, a frame can be taken and the
    next one in the list can be found. */
struct frame_marker {
    // Pointer to the next frame in the list. NULL if this is the last.
    struct frame_marker *next_frame;
};

/*  Read Limine's memory map, and construct an initial linked list of available
    memory blocks from this.

    Returns a negative value if it failed, in which case the system should just
    halt.
    On success, returns the number of blocks found that are usable right now. */
unsigned long int mem_init();

/*  Take the first available block of physical memory (4KB) from the linked list
    of blocks (essentially, `first_page_frame`), and update the linked list
    accordingly */
void *get_phys_block();

/*  Add a block of physical memory back to the linked list so that it can be
    reused. Memory in a free'd block shouldn't be used unless it's allocated
    again */
void put_phys_block(void *);

/*  Attempt to find `n` consecutive blocks of memory, and reserve them, returning
    the address of the first of the blocks. If there are no suitable blocks to be
    found, return NULL. */
void *get_consecutive_phys_blocks(unsigned int n);

#endif