#ifndef __GENERIC_MEM_H__
#define __GENERIC_MEM_H__

#include "flux.h"
#include <stdint.h>

struct mmap_region {
    flux_phyaddr base;
    uint64_t length;
    uint8_t type;
};

void init_phys_allocator();

#endif /* __GENERIC_MEM_H__ */