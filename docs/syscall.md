# Syscalls

System calls, or syscalls, are user applications' main way of interacting with the kernel. The specific way of invoking a syscall is different, but similar, depending on the system architecture. For x64, it's done by generating a software interrupt with vector 0x32. The particular function to call is specified in the `rax` 64 bit register, and the parameters

|---------|----------|-------------|--------------|---------------|-------------|---------------------------------|
| Name    | rax      | rsi         | rdi          | r8            | r9          | Description                     |
|---------|----------|-------------|--------------|---------------|-------------|---------------------------------|
| send    | 0        | void *data  | int64_t type | int64_t dest  | int64_t src |                                 |