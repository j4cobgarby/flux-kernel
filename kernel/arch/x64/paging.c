#include "flux.h"

#include "generic/debug.h"
#include "paging.h"
#include "arch/x64/mem.h"
#include "arch/x64/structures.h"
#include "serial.h"
#include "generic/printk.h"

#include <cpuid.h>

pml4_entry_t *kernel_pml4_table = NULL;

register_cr3 read_cr3() {
    flux_phyaddr ret;
    __asm__ volatile    ("mov %%cr3, %0"
                        : "=r"(ret));
    return ret;
}

void set_cr3(register_cr3 val) {
    __asm__ volatile    ("mov %0, %%cr3"
                        :
                        : "r"(val));
}

void zero_paging_structure(uint64_t *table) {
    for (int i = 0; i < 512; i++) {
        table[i] = 0x0;
    }
}

void paging_init() {
    /*  At this point we expect that mem_init has been called already, so the
        kernel's paging structure should already be set up starting at
        kernel_pml4_table. */

    set_cr3(CR3_BASE_ADDR((flux_phyaddr)kernel_pml4_table));
}

/*  This function "maps" a virtual address to a physical address using the 4-level
    paging scheme. It works by starting at the top level paging structure table
    (the pml4 table) and getting the entry at a certain index into that table. The
    index is given by certain bits of the virtual address that we wish to map. We
    then check to see if that entry is "present". If it isn't, that means that it's
    not pointing to any paging structure, and so we have to create one. Paging
    structures are all 4K bytes long, so we can allocate a single physical block for
    it. We then update the entry in the table we're currently in to be present, and
    to point to the new block we just allocated. Whether or not the entry was
    present to begin with, it certainly is at this point, so we repeat the process
    starting now with the table pointed to by the current entry, until we get to
    the final page table. When we do, we set the relevant pointer in the page table
    to point to the physical address we want to map to. */
void map_page_specific_pml4(pml4_entry_t *pml4_addr, flux_phyaddr paddr, flux_virtaddr vaddr, unsigned int flags) {
#ifdef DEBUG_PRINT_ON_PAGE_MAP
    printk(SYSTEM_PAGING "Mapping vaddr 0x%p to paddr 0x%p\n", vaddr, paddr);
#endif

    // To find the index in the PML4 which is responsible for, extract the index
    // from the virtual address.
    int pml4_i = EXTRACT_PML4_INDEX(vaddr);
    int pdpt_i = EXTRACT_PDPT_INDEX(vaddr);
    int pdt_i = EXTRACT_PDT_INDEX(vaddr);
    int pt_i = EXTRACT_PT_INDEX(vaddr);

    // CR3 points to the currently active PML4 table. We assume here that it points
    // to a valid table. Each paging table (in 64 bit mode, which we're using here)
    // has 512 64-bit entries.
    pml4_entry_t *pml4_e = &(pml4_addr[pml4_i]);
    if (!(*pml4_e & PSE_PRESENT)) {
        // If the entry we want doesn't currently exist, then that means the corresponding
        // Page Directory Pointer Table also doesn't exist. First we allocate a 4K block for
        // that, and then construct the PML4 entry to point to it.
        flux_phyaddr new_pdpt = (flux_phyaddr)get_phys_block();
#ifdef DEBUG_PRINT_ON_NEW_PAGE_STRUCTURE
        printk(SYSTEM_PAGING "Allocated new PDP table at %p\n", new_pdpt);
#endif
        zero_paging_structure((uint64_t*)new_pdpt);
        *pml4_e = PSE_PTR(new_pdpt) | PSE_PRESENT | flags;
    } else {
    }

    pdpt_entry_t *pdpt_e = &(((pdpt_entry_t*)PSE_PTR(*pml4_e))[pdpt_i]);
    if (!(*pdpt_e & PSE_PRESENT)) {
        flux_phyaddr new_pdt = (flux_phyaddr)get_phys_block();
#ifdef DEBUG_PRINT_ON_NEW_PAGE_STRUCTURE
        printk(SYSTEM_PAGING "Allocated new Page Directory Table at %p\n", new_pdt);
#endif
        zero_paging_structure((uint64_t*)new_pdt);
        *pdpt_e = PSE_PTR(new_pdt) | PSE_PRESENT | flags;
    }

    pdt_entry_t *pdt_e = &(((pdt_entry_t*)PSE_PTR(*pdpt_e))[pdt_i]);
    if (!(*pdt_e & PSE_PRESENT)) {
        flux_phyaddr new_pt = (flux_phyaddr)get_phys_block();
#ifdef DEBUG_PRINT_ON_NEW_PAGE_STRUCTURE
        printk(SYSTEM_PAGING "Allocated new page table at %p\n", new_pt);
#endif
        zero_paging_structure((uint64_t*)new_pt);
        *pdt_e = PSE_PTR(new_pt) | PSE_PRESENT | flags;
    }

    pt_entry_t *pt_e = &(((pt_entry_t*)PSE_PTR(*pdt_e))[pt_i]);
    *pt_e = PSE_PTR(paddr) | PSE_PRESENT | flags;
}

void map_page(flux_phyaddr paddr, flux_virtaddr vaddr, unsigned int flags) {
    map_page_specific_pml4((pml4_entry_t*)CR3_BASE_ADDR(read_cr3()), paddr, vaddr, flags);
}