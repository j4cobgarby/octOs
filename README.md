![Oct Os Logo](https://github.com/j4cobgarby/octOs/blob/master/Logo%20OctOs%20Sketch.png)

# oct kernel and related software

    - booted with the GRUB2 bootloader.
    - runs on i686 (x86-32) architecture.

I've made two kernels in the past but every time I make one I learn things, so
here's another. 

## Documentation

I've written about various aspects of the operating system in the [docs](/docs)
directory.

## Building

You need the following software:

### Required software

#### For compiling
- An i686 gcc cross-compiler. (e.g. i686-elf-gcc) (to compile C source files for the CPU)
- grub. (to generate the grub image)
- nasm. (to assemble the assembly source files)
- i686-elf-binutils.

#### For creating the disk image
- sfdisk (for partitioning a disk for the system to run on)
- dosfstools (for making the FAT filesystem)
    
### Optional software

- bochs and/or qemu (for emulating the system)
- bear (for `make compilecommands`, used for clangd when writing code)

### Instructions

Once you have all of this software, the build steps are:

1. Clone this repository.
2. cd into the repo directory.
3. type `make`.

And then it should all compile successfully. If you want to emulate it, type `make bochsrun` or `make qemurun` depending on your preference.
