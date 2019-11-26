console_row dd 0
console_col dd 0

isr_syscall:
    cli

    cmp eax, 0
    jne .s1 ; skip next line
    call syscall_console_write
    iret
.s1:
    cmp eax, 1
    jne .s2
    call syscall_console_reset
    iret
.s2:
    cmp eax, 2
    jne .s3
    call syscall_console_putchar
    iret
.s3:
    cmp eax, 3
    jne .s4
    call syscall_console_movecursor
    iret
.s4:
    cmp eax, 4
    jne .s5
    call syscall_console_setcursor
    iret
.s5:
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
    mov byte bl, [ebx] ; putchar [ebx]
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
    mov dword [console_row], 0 ; reset cursor
    mov dword [console_col], 0
    popa
    ret

; EAX=2 BL=char
syscall_console_putchar:
    pusha

    cmp bl, 10 ; newline
    je .newline
    jmp .otherwise
.newline:
    inc dword [console_row]
    mov dword [console_col], 0
    jmp .end
.otherwise:
    mov ecx, [console_row]
    imul ecx, 80
    add ecx, [console_col]
    shl ecx, 1  ; ecx *= 2

    add ecx, SCREEN_MIN
    mov byte [ecx], bl

    mov eax, 3
    mov ebx, 3
    int 0x80
.end:
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
    sub dword [console_row], 1
    jmp .end
.down:
    add dword [console_row], 1
    jmp .end
.left:
    sub dword [console_col], 1
    jmp .end
.right:
    add dword [console_col], 1
    jmp .end
.end:
    ;; Now, clamp the position to the bounds of the screen
    cmp dword [console_row], 0
    jge .rownotless
    mov dword [console_row], 0
.rownotless:
    cmp dword [console_col], 0
    jge .colnotless
    mov dword [console_row], 0
.colnotless:
    cmp dword [console_row], (SCREEN_ROWS-1)
    jle .rownotmore
    mov dword [console_row], (SCREEN_ROWS-1)
.rownotmore:
    cmp dword [console_col], (SCREEN_COLS-1)
    jle .colnotmore
    mov dword [console_col], (SCREEN_COLS-1)
.colnotmore:
    popa
    ret

; EAX=4, EBX=row, ECX=column
syscall_console_setcursor:
    mov dword [console_row], ebx
    mov dword [console_col], ecx
    ret