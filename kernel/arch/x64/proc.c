#include "proc.h"
#include "arch/x64/mem.h"
#include "arch/x64/paging.h"
#include "arch/x64/structures.h"
#include "flux.h"
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
rflags:\t0x%p\n\
cr3:\t0x%p\n", r->rax, r->rbx, r->rcx, r->rdx, r->rsi, r->rdi, r->rsp, r->rbp, 
    r->r8, r->r9, r->r10, r->r11, r->r12, r->r13, r->r14, r->r15,
    r->rip, r->cs, r->ds, r->ss, r->rflags, r->cr3);
}

int init_task_space_from_mem(pml4_entry_t **pml4, uint64_t pmem_offset, uint64_t n_blocks) {
    printk("pml4 = %p; *pml4 = %p\n", pml4, *pml4);

    void *stack_block_start;

    printk("Getting block for pml4\n");
    *pml4 = get_phys_block();
    if (!*pml4) return -1;

    printk("Getting block for stack\n");
    stack_block_start = get_phys_block();
    if (!stack_block_start) return -1;

    printk("Mapping task stack\n");
    // Map the stack to just below kernel (grows downward)
    map_page_specific_pml4(*pml4, 
        (flux_phyaddr)stack_block_start, 
        (flux_virtaddr)__FLUX_BASE_VIRT - 4096, 
        PSE_WRITEABLE | PSE_USER_MODE);

    printk("Mapping task's executable\n");
    // Map the executable's pmem to start at 0x0.
    for (uint64_t bn = 0; bn < n_blocks; bn++) {
        map_page_specific_pml4(*pml4, pmem_offset + 4096 * bn, 4096 * bn, PSE_USER_MODE);
    }
}