; This will contain the init procedure for userspace

msg db "Hello, world!", 0

global userspace_entry
userspace_entry:
    
    ;xchg bx, bx
    mov eax, 1
    int 0x80

    mov eax, 0
    mov ebx, msg
    int 0x80

    jmp $