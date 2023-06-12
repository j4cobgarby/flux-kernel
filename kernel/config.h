#ifndef __INCLUDE_CONFIG_H__
#define __INCLUDE_CONFIG_H__

// Set if the serial terminal which Flux is outputting to is capable of interpreting
// ANSI colour escape sequences.
#define PRINTK_COLOUR

// Set if Flux should print a big ASCII art logo after initialisation.
// #define PRETTY_LOGO

// Set if Flux should print each region of memory that it is told about by Limine.
// #define DEBUG_PRINT_MEMMAP_ENTRIES

// Set if Flux should print a message each time a physical block of memory is
// allocated.
// #define DEBUG_SHOW_PMEM_ALLOCATIONS

// Set if Flux should print a message each time a virtual address is mapped to
// a physical page.
//#define DEBUG_PRINT_ON_PAGE_MAP

// Set if Flux should print a message each time a page mapping function leads to a
// newly allocated page structure.
//#define DEBUG_PRINT_ON_NEW_PAGE_STRUCTURE

#endif /* __INCLUDE_CONFIG_H__ */