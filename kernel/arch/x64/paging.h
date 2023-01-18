#ifndef __ARCH_X64_PAGING_H__
#define __ARCH_X64_PAGING_H__

#include <stdint.h>
#include "arch/x64/structures.h"
#include "flux.h"

#ifdef PRINTK_COLOUR
#define PAGING "\x1b[36m( PAGE) \x1b[37m"
#else
#define PAGING "( PAGE) "
#endif

register_cr3 read_cr3();

void map_page(flux_phyaddr paddr, flux_virtaddr vaddr, unsigned int flags);

#endif /* __ARCH_X64_PAGING_H__ */