#include "arch/x64/proc.h"
#include "flux.h"

#include <stdint.h>
#include <stddef.h>

#include "limine.h"

#include "generic/debug.h"
#include "generic/printk.h"
#include "generic/scheduler.h"

#ifdef __ARCH_X64__
#include "arch/x64/paging.h"
#include "arch/x64/structures.h"
#include "arch/x64/idt.h"
#include "arch/x64/mem.h"
#include "arch/x64/serial.h"
#include "arch/x64/gdt.h"
#else
#error Unkown architecture
#endif

static volatile struct limine_module_request mod_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
    
};

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void loop(void) {
    for (;;) {
        __asm__("xchg %bx, %bx");
    }
}

void _start(void) {
    // Some architecture-specific initialisation
#ifdef __ARCH_X64__
    /*  We initialise the serial console first so that, as soon as possible, we can
        send debugging information to the host operating system (if running in a VM),
        or an external computer (if running on hardware). */
    serial_init();
    /*  Here we initialise the _physical_ memory management system. This deals with
        allocating and freeing individual blocks of physical memory, and doesn't
        deal at all with virtual address translation. */
    mem_init();
    paging_init();
    /*  Although Limine actually loads a valid GDT for us, we want to load and have
        control over our own. This way, we can add entries to it when needed. */
    gdt_init();
    /*  Early on we need to initialise the IDT. This describes the code that is run
        when the CPU encounters various interrupts and exceptions. */
    idt_init();
#endif /* __ARCH_X64__ */
    scheduler_init();

    printk("Trying to get block?");
    get_phys_block();
    printk("Done.\n");

    struct limine_module_response *mod_response = mod_request.response;
    printk("Limine modules loaded with %d modules.\n", mod_response->module_count);

    for (int i = 0; i < mod_response->module_count; i++) {
        struct limine_file *file = mod_response->modules[i];
        pml4_entry_t *task_pml4;

        printk("Initialising task memory layout.\n");

        printk("Task pmem begins at %p\n", (uint64_t)file->address - (uint64_t)__FLUX_BASE_VIRT);
        init_task_space_from_mem(&task_pml4, (uint64_t)file->address - (uint64_t)__FLUX_BASE_VIRT, 1);

        printk("Calling module code '%s' @ 0x%p\n", file->path, file->address);

        void (*mod_exe)(void) = file->address;
        __asm__ ("xchg %bx, %bx");
        segment_selector_t cs = INIT_SEGMENT_SELECTOR(3, 0, 3);
        segment_selector_t ss = INIT_SEGMENT_SELECTOR(3, 0, 4);
        __asm__(
"\
push %0\n\
push %%rsp\n\
pushf\n\
push %1\n\
push %2\n\
iretq"
    :
    : "r" ((uint64_t)ss), "r" ((uint64_t)cs), "r" (mod_exe));

        // mod_exe();
    }

    printk(PGOOD("Flux Kernel " FLUX_VERS_STRING " initialised successfully!\n"));

    
#ifdef PRETTY_LOGO
    printk("\
                     .'                                   ..                    \n\
                .'.  oKkl.                           .'.  ,;;,.                 \n\
             .;lxk;  oNXX:                         ;dOO,  .;:::.                \n\
         .,:okOOOx'  oNXX:                        .kXX0,   .:;:,     ..         \n\
      .;lxOOOOxl;.   oNXX:                 ..     .kXX0,    ,:;:.   ;;:;'       \n\
   .:okOOOkoc,.      oNXX:             .:lx0:     .kXX0,    .:::;  ,::::;       \n\
   l0OOkl;.          oNXX:             oNNXK:     .kXX0,     ':;:',:;;:;        \n\
   lO0Ol         .   oNXX:             dNNXK:     .kXX0,      ;:::::::;.        \n\
   oO0Oc     .':ox,  oNNX:             dNNXK:     .kXX0,      .:;;;;:;.         \n\
   oO0Oc  .;ldkOOk,  oNXX:             dNNXK:     .kXX0,      .;:;;:;.          \n\
   oO0Od:okOOOkdc,   oNXXc             dNNXX:     .kXX0,      ,:::;:.           \n\
   oO0OOOOOxl;.      :xk0c             dNNXX:     .kXX0,     ,::::::,           \n\
   l0OOkdc,.         :cccl:,.          dNNXX:     'kK0k,    ':;;:;:;:.          \n\
   lO0Ol             .,:ccccl:;'.     .dNNXK:  .,cdOOOk,   ':;,;;':;:;          \n\
   oO0Oc                .';clcccc:,.  .dNNXKo;lxOOOOdl,.  ':;,;:. ':;:'         \n\
   oO0Oc                   .',:ccccl' .oNXK0OOOOko:'.     .;;::.   ;:;:.        \n\
   oO0Oc                       .,;co' .lK0kOOxl;.            ..    .:;:,        \n\
   oKxl.                          ... .lKOo:'.                      ,:;:.       \n\
   ..                                  .,.                          .';;:.      \n\
                                                                       ...      \n");
#endif

    done();
}