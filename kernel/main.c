#include <stdint.h>
#include <stddef.h>
#include "arch/x64/paging.h"
#include "arch/x64/structures.h"
#include "limine.h"
#include "flux.h"

#ifdef __ARCH_X64__
#include "arch/x64/idt.h"
#include "arch/x64/mem.h"
#include "arch/x64/serial.h"
#else
#error Unkown architecture
#endif

volatile struct limine_terminal_request flux_terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_address_request kern_addr_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
#ifdef __ARCH_X64__
    // Initialise the IDT, which the CPU uses to translate interrupt/exception
    // vectors to the address of the handler of that interrupt.
    serial_init();
    idt_init();
    mem_init();
#endif /* __ARCH_X64__ */

    if (flux_terminal_request.response == NULL
        || flux_terminal_request.response->terminal_count < 1) {
        done();
    }

    if (kern_addr_request.response == NULL) done();

    LIMINE_WRITE("\x1b[32mWelcome to flux!\x1b[37m\n", 27);

    done();
}
