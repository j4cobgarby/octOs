CC = i686-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding

C_OBJS := \
kio.o \
pmm.o \
asm_procs.o \

all: octos.iso

clear:
	rm -rf build

octos.iso: $(OBJECTS)
