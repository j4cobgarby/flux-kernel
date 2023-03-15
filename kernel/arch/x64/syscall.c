#include "flux.h"

#include "generic/debug.h"
#include "idt.h"

INTERRUPT_HANDLER isr_SYSCALL(struct isr_frame *frame) {
    ERROR("SYSCALL not implemented!");
}