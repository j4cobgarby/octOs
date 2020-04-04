void entry_in_c(void) {
    asm("xchg %bx, %bx;\
         mov $5, %eax;\
         int $0x80;");
}