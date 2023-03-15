.global save_regs
.global load_regs
.extern load_regs
.extern save_regs
.type save_regs, @function

save_regs:
    push %rbp
    mov %rsp, %rbp
    
    # Store the general-purpose registers
    mov %rax,   (%rdi)
    mov %rbx,   8(%rdi)
    mov %rcx,   16(%rdi)
    mov %rdx,   24(%rdi)
    mov %rsi,   32(%rdi)
    mov %rdi,   40(%rdi)
    mov %rsp,   48(%rdi)
    mov %rbp,   56(%rdi)
    mov %r8,    64(%rdi)
    mov %r9,    72(%rdi)
    mov %r10,   80(%rdi)
    mov %r11,   88(%rdi)
    mov %r12,   96(%rdi)
    mov %r13,   104(%rdi)
    mov %r14,   112(%rdi)
    mov %r15,   120(%rdi)

    # Store the instruction pointer
    lea (%rip), %rax
    mov %rax,   128(%rdi)

    # Store the segment registers
    movw %cs,   136(%rdi)
    movw %ds,   138(%rdi)
    movw %ss,   140(%rdi)
    movw %es,   142(%rdi)
    movw %fs,   144(%rdi)
    movw %gs,   146(%rdi)

    # Store the RFLAGS register
    pushfq
    pop %rax
    mov %rax,   152(%rdi)

    # Store the CR3 register
    mov %cr3,   %rax
    mov %rax,   160(%rdi)

    mov %rbp,   %rsp
    pop %rbp
    ret

load_regs:
    push %rbp
    mov %rsp,   %rbp

    xchg %bx, %bx

    # Load RFLAGS
    mov 152(%rdi), %rax
    push %rax
    popfq

    # Load CR3
    mov 160(%rdi), %rax
    mov %rax, %cr3

    # Load general purpose regs
    mov 8(%rdi), %rbx
    mov 16(%rdi), %rcx
    mov 24(%rdi), %rdx
    mov 32(%rdi), %rsi
    mov 40(%rdi), %rdi
    mov 48(%rdi), %rsp
    mov 56(%rdi), %rbp
    mov 64(%rdi), %r8
    mov 72(%rdi), %r9
    mov 80(%rdi), %r10
    mov 88(%rdi), %r11
    mov 96(%rdi), %r12
    mov 104(%rdi), %r13
    mov 112(%rdi), %r14
    mov 120(%rdi), %r15

    # Load segment registers
    // movw 136(%rdi), %cs
    // movw 138(%rdi), %ds
    // movw 140(%rdi), %ss
    // movw 142(%rdi), %es
    // movw 144(%rdi), %fs
    // movw 146(%rdi), %gs

    cli
    # Push RFLAGS
    pushfq
    # Push CS
    #movw 136(%rdi), %ax
    
    pushw 0x08
    # Push saved instruction pointer as iret return address
    mov 128(%rdi), %rax
    push %rax
    mov (%rdi), %rax

    iretq
