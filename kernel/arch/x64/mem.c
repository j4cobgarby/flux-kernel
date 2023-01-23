#include "arch/x64/structures.h"
#include "flux.h"

#include <stdint.h>
#include <stdlib.h>

#include "serial.h"
#include "paging.h"
#include "generic/debug.h"
#include "generic/printk.h"
#include "mem.h"
#include "limine.h"

struct frame_marker *first_page_frame;

static volatile struct limine_memmap_request memmap_request= {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
};

int find_free_frames() {
    unsigned long int ret = 0;
    struct limine_memmap_response *resp = memmap_request.response;

    for (uint64_t i = 0; i < resp->entry_count; i++) {
        struct limine_memmap_entry *entry = resp->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t b_offset = 0; b_offset < entry->length; b_offset += 4096) {
                struct frame_marker *new_block = (struct frame_marker*)(entry->base + b_offset);

                // Put the metadata in the new block, saying where the next block in the linked
                // list is located.
                new_block->next_frame = first_page_frame;

                // Make the linked list start at the new block.
                first_page_frame = new_block;

                ret++;
            }
        }
    }

    return ret;
}

void setup_kernel_map() {
    struct limine_memmap_response *resp = memmap_request.response;

    for (uint64_t i = 0; i < resp->entry_count; i++) {
        struct limine_memmap_entry *entry = resp->entries[i];

        if (entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES) {
            for (uint64_t b_offset = 0; b_offset < entry->length; b_offset += 4096) {
                // We want to map the kernel's physical memory to the higher half memory which Limine
                // set it up at previously. This starts at __FLUX_BASE_VIRT, which is defined by the
                // linker script
                map_page_specific_pml4(kernel_pml4_table, 
                    entry->base + b_offset, 
                    (flux_virtaddr)__FLUX_BASE_VIRT + b_offset, 
                    PSE_WRITEABLE);
            }
        }
    }

    // Map the first 4GiB of physical memory to virtual memory starting at the
    // start of higher half memory
    
    struct limine_hhdm_response *hhdm_response = hhdm_request.response;
    printk("HHDM Offset = 0x%p\n", hhdm_response->offset);

    for (uint64_t p = 0; p < 0x100000000; p += 4096) {
        map_page_specific_pml4(kernel_pml4_table, p, hhdm_response->offset + p, PSE_WRITEABLE);
    }
}

unsigned long int mem_init() {
    struct limine_memmap_response *resp = memmap_request.response;
    
    first_page_frame = NULL;

    ASSERT(resp != NULL);

    printk(SYSTEM_PMEM "Initialising physical memory manager\n");

#ifdef DEBUG_PRINT_MEMMAP_ENTRIES
    /*  Enumerate the memory map entries that we requested from Limine. */
    for (uint64_t i = 0; i < resp->entry_count; i++) {
        struct limine_memmap_entry *entry = resp->entries[i];

        printk(SYSTEM_PMEM "Memory map entry: base: 0x%p, length: 0x%p, type: %d\n", 
            entry->base, entry->length, entry->type);
    }
#endif

    unsigned long int newblock_count = find_free_frames();
    printk(SYSTEM_PMEM PGOOD("Added %d 4K blocks to the physical page frame list.\n"), newblock_count);

    kernel_pml4_table = get_phys_block();
    setup_kernel_map();
    printk(SYSTEM_PAGING PGOOD("Successfully set up kernel page mapping.\n"));

    return newblock_count;
}

void *get_phys_block() {    
    // Get the first available page frame
    void *ret = (void*)first_page_frame;

    ASSERT(ret != NULL);

    first_page_frame = first_page_frame->next_frame;

#ifdef DEBUG_SHOW_PMEM_ALLOCATIONS
    printk(SYSTEM_PMEM "Allocated new physical block at 0x%p\n", ret);
#endif

    return ret;  
}

void put_phys_block(void *b_addr) {
    struct frame_marker *new_block = (struct frame_marker *)b_addr;

    *new_block = (struct frame_marker) {
        .next_frame = first_page_frame
    };

    first_page_frame = new_block;
}

void *get_consecutive_phys_blocks(unsigned int n) {
    ERROR("Function not implemented!"); // TODO
}