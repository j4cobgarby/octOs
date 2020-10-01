; This will contain the init procedure for userspace

welcome db "Hello from userspace assembly",10,0

extern entry_in_c

global userspace_entry
userspace_entry:
    mov eax, 0
    mov ebx, welcome
    int 0x80

    call entry_in_c

    jmp $