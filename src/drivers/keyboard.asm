isr_keyboard:
    pusha
    cli

    in al, 0x60

    mov al, PIC_EOI
    out PIC1_CMD, al

    popa
    iret

init_keyboard:
    pusha
    mov eax, isr_keyboard   ; eax = 0xUUUULLLL, U=upper L=lower
    mov word [idt_keyboard], ax ; put the lower 16 bits into the IDT
    shr eax, 16 ; set ax to the upper bits of eax
    mov word [idt_keyboard + 6], ax ; put the upper 16 bits into the IDT with an offset
    popa
    ret