# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := flux.elf

CC = gcc
LD = ld

# User controllable CFLAGS.
CFLAGS ?= -g -O2 -Wall -Wextra

# User controllable preprocessor flags. We set none by default.
CPPFLAGS ?=

# User controllable nasm flags.
NASMFLAGS ?= -F dwarf -g

# User controllable linker flags. We set none by default.
LDFLAGS ?=

# Internal C flags that should not be changed by the user.
override CFLAGS +=       \
    -std=c11             \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-lto             \
    -fno-pie             \
    -fno-pic             \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-mmx             \
    -mno-sse             \
    -mno-sse2            \
    -mno-red-zone        \
    -mcmodel=kernel      \
    -I.                  \
    -Wno-unused-parameter\
    -Wno-unused-function \
    -D__ARCH_$(ARCH)__   \

# Internal linker flags that should not be changed by the user.
override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -m elf_x86_64           \
    -z max-page-size=0x1000 \
    -T linker.ld            \
    -no-pie


# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -f elf64

# Use find to glob all *.c, *.S, and *.asm files in the directory and extract the object names.
override CFILES := $(shell find . -type f -name '*.c')
override ASFILES := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')
override OBJ := $(CFILES:.c=.o) $(ASFILES:.S=.o) $(NASMFILES:.asm=.o)
override HEADER_DEPS := $(CFILES:.c=.d) $(ASFILES:.S=.d)

# Default target.
.PHONY: all
all: $(KERNEL)

limine.h:
	curl https://raw.githubusercontent.com/limine-bootloader/limine/trunk/limine.h -o $@

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

# Compilation rules for *.c files.
%.o: %.c limine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Compilation rules for *.S files.
%.o: %.S limine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Compilation rules for *.asm (nasm) files.
%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ)

.PHONY: distclean
distclean: clean
	rm -f limine.h
