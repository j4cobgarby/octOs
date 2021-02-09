all:
	cd subprojects/kernel && make

clean:
	cd subprojects/kernel && make clean

compilecommands:
	bear -- make

bochsrun:
	bochs

qemurun:
	qemu-system-i386 build/disk.img
