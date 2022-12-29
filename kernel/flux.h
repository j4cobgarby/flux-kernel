#ifndef __INCLUDE_FLUX_H__
#define __INCLUDE_FLUX_H__

#include "limine.h"
#include <stdint.h>

extern volatile struct limine_terminal_request flux_terminal_request;
#define LIMINE_WRITE(msg, len) flux_terminal_request.response->write(flux_terminal_request.response->terminals[0], msg, len)

#ifdef __ARCH_X64__
typedef uint64_t flux_phyaddr;
typedef uint64_t flux_virtaddr;
#endif

extern char __FLUX_BASE_VIRT[];
extern char __FLUX_BSS_END[];

// Some helper functions useful at inititialisation
void itoa_hex64(char *buf, uint64_t num);

#endif /* __INCLUDE_FLUX_H__ */