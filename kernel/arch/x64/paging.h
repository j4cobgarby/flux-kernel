#ifndef __ARCH_X64_PAGING_H__
#define __ARCH_X64_PAGING_H__

#include "flux.h"

#include <stdint.h>
#include "arch/x64/structures.h"

extern pml4_entry_t *kernel_pml4_table;

register_cr3 read_cr3();
void set_cr3(register_cr3 val);

void paging_init();

void map_page(flux_phyaddr paddr, flux_virtaddr vaddr, unsigned int flags);
void map_page_specific_pml4(pml4_entry_t *pml4_addr, flux_phyaddr paddr, flux_virtaddr vaddr, unsigned int flags);

#endif /* __ARCH_X64_PAGING_H__ */