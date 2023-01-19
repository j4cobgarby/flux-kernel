#ifndef __INCLUDE_FLUX_H__
#define __INCLUDE_FLUX_H__

#include <stdint.h>
#include "limine.h"

#include "generic/printk.h"

#define STR_(x) #x
#define STR(x) STR_(x)

#define FLUX_VERS_MAJOR 0
#define FLUX_VERS_MINOR 1
#define FLUX_VERS_STRING "v" STR(FLUX_VERS_MAJOR) "." STR(FLUX_VERS_MINOR)

#ifdef __ARCH_X64__
typedef uint64_t flux_phyaddr;
typedef uint64_t flux_virtaddr;
#endif

extern char __FLUX_BASE_VIRT[];
extern char __FLUX_BSS_END[];

// Some helper functions useful at inititialisation
void itoa_hex64(char *buf, uint64_t num);

#endif /* __INCLUDE_FLUX_H__ */