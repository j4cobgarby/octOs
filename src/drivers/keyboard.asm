isr_keyboard:
    pusha
    cli

    in al, 0x60

    push eax ; putchar
    mov eax, 2
    mov bl, 178
    int 0x80
    pop eax

    ; W = 0x11
    ; S = 0x1f
    ; A = 0x1e
    ; D = 0x20

    cmp al, 0x11
    je .up
    cmp al, 0x1f
    je .down
    cmp al, 0x1e
    je .left
    cmp al, 0x20
    je .right
    jmp .none
.up:
    mov ebx, 0
    jmp .done
.down:
    mov ebx, 1
    jmp .done
.left:
    mov ebx, 2
    jmp .done
.right:
    mov ebx, 3
    jmp .done
.done:
    mov eax, 3
    int 0x80
.none:
    mov al, PIC_EOI
    out PIC1_CMD, al

    popa

    sti
    iret

init_keyboard:
    pusha
    mov eax, isr_keyboard   ; eax = 0xUUUULLLL, U=upper L=lower
    mov word [idt_keyboard], ax ; put the lower 16 bits into the IDT
    shr eax, 16 ; set ax to the upper bits of eax
    mov word [idt_keyboard + 6], ax ; put the upper 16 bits into the IDT with an offset
    popa
    ret