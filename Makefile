all:
	cd subprojects/kernel && make -j$(shell nproc)

clean:
	cd subprojects/kernel && make clean

compilecommands:
	bear -- make

bochsrun:
	bochs

qemurun: build/octos.img
	qemu-system-i386 $^
