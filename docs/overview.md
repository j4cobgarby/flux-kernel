## Flux

This is my latest kernel experiment after OctOS. 

Being a microkernel, Flux only handles:
 - Low level hardware access (memory, busses, etc.)
 - Memory management (getting ram info, allocating pages, etc.)
 - Process management (managing creating and running of processes, sharing
        pages, etc.)
    
Then userspace applications are responsible for useful services such as:
 - Disk access
 - Filesystems
 - Network interfaces

Therefore a full operating system will comprise of the Flux kernel and a number of these userspace applications.