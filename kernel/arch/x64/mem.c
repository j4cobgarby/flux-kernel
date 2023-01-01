#include <stdlib.h>

#include "arch/x64/serial.h"
#include "mem.h"
#include "limine.h"
#include "flux.h"

struct frame_marker *first_page_frame;

static volatile struct limine_memmap_request memmap_request= {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

int mem_init() {
    unsigned int newblock_count = 0;
    struct limine_memmap_response *resp = memmap_request.response;
    
    first_page_frame = NULL;

    if (!resp) return -1;

    com_printf(*primary_com_port, "Initialising physical memory manager\n");

    for (uint64_t i = 0; i < resp->entry_count; i++) {
        struct limine_memmap_entry *entry = resp->entries[i];

#ifdef DEBUG_PRINT_MEMMAP_ENTRIES
        com_printf(*primary_com_port, "Memory map entry: base: 0x%p, length: 0x%p, type: %d\n", 
            entry->base, entry->length, entry->type);
#endif

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            // Usable entries are guaranteed by Limine to be 4096 byte aligned
            // (for base and length).
            // So, now we want to iterate through the 4KB blocks of this entry
            // and add each to our linked list.

            for (uint64_t b_offset = 0; b_offset < entry->length; b_offset += 4096) {
                struct frame_marker *new_block = (struct frame_marker*)(entry->base + b_offset);

                // Put the metadata in the new block, saying where the next block in the linked
                // list is located.
                *new_block = (struct frame_marker) {
                    .next_frame = first_page_frame
                };

                // Make the linked list start at the new block.
                first_page_frame = new_block;

                newblock_count++;
            }
        }   
    }

    com_printf(*primary_com_port, "Added %d 4K blocks to the physical page frame list.\n", newblock_count);

    return newblock_count;
}

void *get_phys_block() {
    // Get the first available page frame
    void *ret = (void*)first_page_frame;

    if (ret != NULL) {
        first_page_frame = first_page_frame->next_frame;
    }

    com_printf(*primary_com_port, "Allocated new physical block at 0x%p\n", ret);

    return ret;  
}

void put_phys_block(void *b_addr) {
    struct frame_marker *new_block = (struct frame_marker *)b_addr;

    *new_block = (struct frame_marker) {
        .next_frame = first_page_frame
    };

    first_page_frame = new_block;
}