CC = i686-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding -Wno-unused-parameter -I include -I ../kernel/include -O2
BUILDDIR=../../build/klib

OBJS = kheap.o \
kmem.o \
atox.o

all: $(OBJS)

clean:
	rm -rf $(BUILDDIR)

%.o: src/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)