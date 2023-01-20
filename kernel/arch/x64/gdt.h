/** gdt.h
 * Although Limine will set up a GDT for us, we want to set up our own so that 
 * we have full control over it.
 *
 * Contributors: (add your name if you modify the file)
 * Jacob Garby <j4cobgarby@gmail.com>
 */

#ifndef __INCLUDE_X64_GDT_H__
#define __INCLUDE_X64_GDT_H__

#include "flux.h"

#include "structures.h"

#define SYSTEM_GDT PACCENT("(  GDT) ")

extern struct gdt_descriptor_long gdt_table[];

extern void load_gdt(void);
extern void reload_seg_registers(void);

void gdt_init();

#endif /* __INCLUDE_X64_GDT_H__ */