; This will contain the init procedure for userspace

welcome db "Welcome to octOs!", 0

global userspace_entry
userspace_entry:    
    mov eax, 1
    int 0x80

    mov eax, 0
    mov ebx, welcome
    int 0x80

    jmp $