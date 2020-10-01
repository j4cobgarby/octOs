void entry_in_c(void) {
    asm volatile(
        "mov $0, %%eax;\
         int $0x80;"
        :
        : "b" ("Hello from userspace C\n\0")
        : "eax");
}