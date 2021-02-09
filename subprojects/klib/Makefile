CC = i686-elf-gcc
CFLAGS = -Wall -Wextra -ffreestanding -I include -I ../kernel/include -O2
BUILDDIR=../../build/klib

OBJS = kheap.o

all: $(OBJS)

clean:
	rm -rf $(BUILDDIR)

%.o: src/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c -o $(BUILDDIR)/$@ $< $(CFLAGS)