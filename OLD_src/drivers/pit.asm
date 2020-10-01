PIT_CHANNEL0_DATA equ 0x40
PIT_CHANNEL1_DATA equ 0x41
PIT_CHANNEL2_DATA equ 0x42
PIT_CMD equ 0x43

init_pit:
    pusha

    ; Configure the ISR for the PIT channel 0 (IRQ0)
    mov eax, isr_pit
    mov word [idt_pit], ax
    shr eax, 16
    mov word [idt_pit + 6], ax

    ; Configure channel 0
    mov al, 00110110b
    out PIT_CMD, al ; configure channel 0 as:
                    ;  - access mode: consecutive low-byte/high-byte
                    ;  - op mode: square wave generator
                    ;  - binary mode
    
    mov al, 11111111b
    out PIT_CHANNEL0_DATA, al
    mov al, 11111111b
    out PIT_CHANNEL0_DATA, al ; about 18Hz after all division

    popa
    ret

isr_pit:
    pusha

    mov al, 0x20 ; acknowledge interrupt to the PIT
    out 0x20, al

    popa
    iret