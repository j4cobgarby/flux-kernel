## Flux

This is my latest kernel experiment after OctOS. Flux is a microkernel that primarily targets x86-64 systems, with the intention of supporting more architectures in the future. I'm developing flux with an emphasis on code readbility and ease of understanding. Hopefully, it will be clear enough that people with very little understanding of operating system development can understand more by looking through (and perhaps writing new) flux code.

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

See the subdirectory `docs` for more information about how flux works.