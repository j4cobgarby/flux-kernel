#include "thread.h"
#include "generic/printk.h"

void print_regs(struct processor_regs * r) {
    printk("\
rax:\t0x%p\n\
rbx:\t0x%p\n\
rcx:\t0x%p\n\
rdx:\t0x%p\n\
rsi:\t0x%p\n\
rdi:\t0x%p\n\
rsp:\t0x%p\n\
rbp:\t0x%p\n\
r8:\t0x%p\n\
r9:\t0x%p\n\
r10:\t0x%p\n\
r11:\t0x%p\n\
r12:\t0x%p\n\
r13:\t0x%p\n\
r14:\t0x%p\n\
r15:\t0x%p\n\
rip:\t0x%p\n\
cs:\t0x%x\n\
ds:\t0x%x\n\
ss:\t0x%x\n\
es:\t0x%x\n\
fs:\t0x%x\n\
gs:\t0x%x\n\
rflags:\t0x%p\n\
cr3:\t0x%p\n", r->rax, r->rbx, r->rcx, r->rdx, r->rsi, r->rdi, r->rsp, r->rbp, 
    r->r8, r->r9, r->r10, r->r11, r->r12, r->r13, r->r14, r->r15,
    r->rip, r->cs, r->ds, r->ss, r->es, r->fs, r->gs, r->rflags, r->cr3);
}