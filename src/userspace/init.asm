; This will contain the init procedure for userspace

welcome db "Welcome to octOs!",10,"This is a test", 0

extern entry_in_c

global userspace_entry
userspace_entry:
    mov eax, 0
    mov ebx, welcome
    int 0x80

    xchg bx, bx

    call entry_in_c

    jmp $
    ; 100780
    ;100591