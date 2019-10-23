; This will contain the init procedure for userspace

global userspace_entry
userspace_entry:
    xchg bx, bx
    cli
    jmp $