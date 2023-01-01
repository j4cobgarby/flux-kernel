#ifndef __ARCH_X64_PAGING_H__
#define __ARCH_X64_PAGING_H__

#include <stdint.h>
#include "arch/x64/structures.h"
#include "flux.h"

register_cr3 read_cr3();

void map_page(flux_phyaddr paddr, flux_virtaddr vaddr, unsigned int flags);

#endif /* __ARCH_X64_PAGING_H__ */