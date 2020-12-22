all:
	cd subprojects/kernel && make -j6

clean:
	cd subprojects/kernel && make clean

compilecommands:
	bear -- make
