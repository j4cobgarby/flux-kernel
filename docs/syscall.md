# Syscalls

System calls, or syscalls, are user applications' main way of interacting with the kernel. The specific way of invoking a syscall is different, but similar, depending on the system architecture. For x64, it's done by generating a software interrupt with vector 0x32. The particular function to call is specified in the `rax` 64 bit register, and the parameters

| Name      | rax       | rsi           | rdi           | r8            | r9            | Description                     |
|-----------|-----------|---------------|---------------|---------------|---------------|---------------------------------|
| send      | 0         | int64_t dest  | int64_t type  | void *data    | int n         | (Non-blocking) sends a message to a process with id `dest`. The message is copied from `data`, and is `n` bytes long.|
| recv      | 1         | int64_t from  | 


