#include "flux.h"

#include "generic/debug.h"
#include "generic/printk.h"
#include "idt.h"

INTERRUPT_HANDLER isr_DE(struct isr_frame *frame){
    ERROR("DE");
}

INTERRUPT_HANDLER isr_NMI(struct isr_frame *frame){
    ERROR("NMI");
}

INTERRUPT_HANDLER isr_BP(struct isr_frame *frame){
    ERROR("BP");
}

INTERRUPT_HANDLER isr_OF(struct isr_frame *frame){
    ERROR("OF");
}

INTERRUPT_HANDLER isr_BR(struct isr_frame *frame){
    ERROR("BR");
}

INTERRUPT_HANDLER isr_UD(struct isr_frame *frame){
    ERROR("UD");
}

INTERRUPT_HANDLER isr_DF(struct isr_frame *frame, uint64_t error_code){
    ERROR("DF");
}

INTERRUPT_HANDLER isr_TS(struct isr_frame *frame, uint64_t error_code){
    ERROR("TS");
}

INTERRUPT_HANDLER isr_NP(struct isr_frame *frame, uint64_t error_code){
    ERROR("NP");
}

INTERRUPT_HANDLER isr_SS(struct isr_frame *frame, uint64_t error_code){
    ERROR("SS");
}

INTERRUPT_HANDLER isr_GP(struct isr_frame *frame, uint64_t error_code){
    __asm__ volatile("xchg %bx,%bx");
    printk("#GP w/ error code %x\n", error_code);
    ERROR("GP");
}

INTERRUPT_HANDLER isr_PF(struct isr_frame *frame, uint64_t error_code){
    ERROR("PF");
}
