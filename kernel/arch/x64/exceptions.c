#include "idt.h"

INTERRUPT_HANDLER isr_DE(struct isr_frame *frame){

}

INTERRUPT_HANDLER isr_NMI(struct isr_frame *frame){

}

INTERRUPT_HANDLER isr_BP(struct isr_frame *frame){

}

INTERRUPT_HANDLER isr_OF(struct isr_frame *frame){

}

INTERRUPT_HANDLER isr_BR(struct isr_frame *frame){

}

INTERRUPT_HANDLER isr_UD(struct isr_frame *frame){

}

INTERRUPT_HANDLER isr_DF(struct isr_frame *frame, uint64_t error_code){

}

INTERRUPT_HANDLER isr_TS(struct isr_frame *frame, uint64_t error_code){

}

INTERRUPT_HANDLER isr_NP(struct isr_frame *frame, uint64_t error_code){

}

INTERRUPT_HANDLER isr_SS(struct isr_frame *frame, uint64_t error_code){

}

INTERRUPT_HANDLER isr_GP(struct isr_frame *frame, uint64_t error_code){

}

INTERRUPT_HANDLER isr_PF(struct isr_frame *frame, uint64_t error_code){

}
