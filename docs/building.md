# Building and Testing

## Configuring the makefile

### Different CPUs

At the moment, oct supports only one CPU architecture (i386). In the future, if more become supported, that will be configurable in the makefile.

### Debug mode

You can set the debug mode of the kernel by setting or not setting the KERNEL_DEBUG symbol. Do this using the `-DKERNEL_DEBUG` flag in the CFLAGS variable at the top of the makefile under subprojects/kernel.