all:
	cd subprojects/kernel && make

clean:
	cd subprojects/kernel && make clean

compilecommands:
	bear -- make

bochsrun: octos.sym.bochs
	bochs

octos.sym.bochs: build/octos.bin
	nm -a build/octos.bin > octos.sym
	awk '!($$2="")' octos.sym > octos.sym.bochs

qemurun:
	qemu-system-i386 build/disk.img
