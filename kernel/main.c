#include <stdint.h>
#include <stddef.h>
#include "arch/x64/mem.h"
#include "limine.h"
#include "flux.h"

#ifdef __ARCH_X64__
#include "arch/x64/idt.h"
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

void itoa_hex64(char *buf, uint64_t num) {
    static char lookup[] = "0123456789abcdef";

    for (int i = 0; i < 16; i++) {
        buf[15-i] = lookup[num & 0xf];
        num >>= 4;
    }
}

void _start(void) {
#ifdef __ARCH_X64__
    // Initialise the IDT, which the CPU uses to translate interrupt/exception
    // vectors to the address of the handler of that interrupt.
    idt_init();
    mem_init();
#endif /* __ARCH_X64__ */

    if (flux_terminal_request.response == NULL
        || flux_terminal_request.response->terminal_count < 1) {
        done();
    }

    if (kern_addr_request.response == NULL) done();

    LIMINE_WRITE("Welcome to flux!\n", 17);

    done();
}
