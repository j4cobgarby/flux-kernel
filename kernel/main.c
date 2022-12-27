#include <stdint.h>
#include <stddef.h>
#include "arch/x64/structures.h"
#include "limine.h"

#ifdef __ARCH_X64__
#include "arch/x64/idt.h"
#else
#error Unkown architecture
#endif

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
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
    idt_init();
#endif /* __ARCH_X64__ */

    if (terminal_request.response == NULL
        || terminal_request.response->terminal_count < 1) {
        done();
    }

    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, "Welcome to flux!", 16);

    __asm__("int $0x32");

    done();
}
