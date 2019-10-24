isr_syscall:
    pusha
    cli

    cmp eax, 0
    jne .s1 ; skip next line
    call syscall_console_write
.s1:cmp eax, 1
    jne .s2
    call syscall_console_reset
.s2:cmp eax, 2
    jne .s3
    call syscall_console_putchar
.s3:
    popa
    sti
    iret

init_syscall:
    pusha
    mov eax, isr_syscall
    mov word [idt_syscall], ax
    shr eax, 16
    mov word [idt_syscall + 6], ax
    popa
    ret

; EAX=0 EBX=char*
syscall_console_write:
    pusha
.loop:
    cmp byte [ebx], 0
    je .end ; is reached null byte, break loop
    push ebx
    mov byte bl, [ebx]
    mov eax, 2
    int 0x80
    pop ebx
    inc ebx
    jmp .loop
.end:
    popa
    ret

; EAX=1
syscall_console_reset:
    pusha
    mov ecx, SCREEN_MIN
.loop:
    mov byte [ecx+1], SCREEN_COLOUR ; clear screen
    mov byte [ecx], 0
    add ecx, 2
    cmp ecx, SCREEN_MAX
    jl .loop
    mov dword [console_cursor_pos], 0 ; reset cursor
    popa
    ret

; EAX=2 BL=char
syscall_console_putchar:
    pusha
    mov ecx, SCREEN_MIN
    add ecx, [console_cursor_pos]
    mov byte [ecx], bl
    add dword [console_cursor_pos], 2
    popa
    ret

; EAX=3 EBX=direction
syscall_console_movecursor:
    pusha

    ; directions:
    ;  0: up
    ;  1: down
    ;  2: left
    ;  3: right
    cmp ebx, 0
    je .up
    cmp ebx, 1
    je .down
    cmp ebx, 2
    je .left
    cmp ebx, 3
    je .right
    jmp .end    ; if none, go to end
.up:
    sub dword [console_cursor_pos], 80 * 2
    jmp .end
.down:
    add dword [console_cursor_pos], 80 * 2
    jmp .end
.left:
    sub dword [console_cursor_pos], 2
    jmp .end
.right:
    add dword [console_cursor_pos], 2
    jmp .end
.end:
    ;; Now, clamp the position to the bounds of the screen
    cmp dword [console_cursor_pos], SCREEN_MIN
    jge .notless
    mov dword [console_cursor_pos], SCREEN_MIN
.notless:
    cmp dword [console_cursor_pos], SCREEN_MAX
    jle .notmore
    mov dword [console_cursor_pos], SCREEN_MAX
.notmore:
    popa
    ret