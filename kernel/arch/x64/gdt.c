#include "flux.h"

#include "gdt.h"
#include "arch/x64/structures.h"
#include "generic/printk.h"

struct gdt_descriptor_long gdt_table[] = {
    INIT_SEG_DESCRIPTOR(0l, 0l, 0, 0),
    INIT_SEG_DESCRIPTOR(0l, 0l, SEG_AB_PRESENT | SEG_AB_DPL(0) | SEG_AB_CODE_DATA | SEG_AB_EXEC | SEG_AB_RW, SEG_FLAG_LONG | SEG_FLAG_4K_BLOCKS),
    INIT_SEG_DESCRIPTOR(0l, 0l, SEG_AB_PRESENT | SEG_AB_DPL(0) | SEG_AB_CODE_DATA | SEG_AB_RW, SEG_FLAG_4K_BLOCKS),
    INIT_SEG_DESCRIPTOR(0l, 0l, SEG_AB_PRESENT | SEG_AB_DPL(3) | SEG_AB_CODE_DATA | SEG_AB_EXEC | SEG_AB_RW, SEG_FLAG_LONG | SEG_FLAG_4K_BLOCKS),
    INIT_SEG_DESCRIPTOR(0l, 0l, SEG_AB_PRESENT | SEG_AB_DPL(3) | SEG_AB_CODE_DATA | SEG_AB_RW, SEG_FLAG_4K_BLOCKS),
};

struct gdtr_image gdtr;

void gdt_init() {
    printk(SYSTEM_GDT "Loading GDT table (at %x).\n", gdt_table);

    gdtr = (struct gdtr_image){
        .limit = sizeof(gdt_table),
        .base = gdt_table,
    };    

    load_gdt();
    reload_seg_registers();
}