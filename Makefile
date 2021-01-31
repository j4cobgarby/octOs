all:
	cd subprojects/kernel && make -j1

clean:
	cd subprojects/kernel && make clean

compilecommands:
	bear -- make

bochsrun:
	bochs

qemurun: build/octos.img
	qemu-system-i386 $^
