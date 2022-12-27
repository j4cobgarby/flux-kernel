# Interrupts

When external hardware attached to the computer need the CPU's attention, or when processes have errors, or when applications need to request the kernel does something, interrupts are used. Their specific mechanism is different depending on the system architecture.

## x64

On x64 systems, all interrupts have an _interrupt vector_ associated with them. Interrupts are broadly categorised into exceptions, and other interrupts. Exceptions happen if a process does something that needs to be sorted out by the kernel, which may result in the process being killed, or the problem being fixed otherwise. This can happen if the process attempts to access memory which they don't have access to, or if they attempt to divide by 0, for two examples.

When an interrupt happens, the CPU will look up the vector in a table in memory called the IDT. Each entry in the IDT maps one vector to an interrupt service routine (ISR), which is a piece of code executed when that interrupt happens. These are also called interrupt handlers.

The files which are relevant to the interrupt system on x64 are:

 - `arch/x64/`
   - `structures.h` - Generic structures needed by the x64 architecture, including the IDT table and the IDT entry.
   - `idt.h`, `idt.c` - Functions to initialise the interrupt system, load an IDT table, and set interrupt handlers. Also includes declerations of these handler functions.
   - `exceptions.c` - Definitions of the interrupt handlers for exceptions.
   - `syscall.c` - Interrupt handler for syscall. Arch specific parts are in this file, but arch-independant syscall logic is in `generic/syscall.c`.