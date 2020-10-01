kterm_row dd 0
kterm_col dd 0

hexdigits db "0123456789abcdef"

kterm_clear:
    pushad
    mov ecx, SCREEN_MIN
.loop:
    mov byte [ecx+1], SCREEN_COLOUR
    mov byte [ecx], 0
    add ecx, 2
    cmp ecx, SCREEN_MAX
    jl .loop
    
    mov byte [kterm_row], 0
    mov byte [kterm_col], 0

    popad
    ret

; eax = C-string's physical memory address
kterm_putstr:
    pushad
    mov ebx, eax
.loop:
    cmp byte [ebx], 0
    je .end
    mov al, [ebx]
    call kterm_putchar
    inc ebx
    jmp .loop
.end:
    popad
    ret

; eax = value to print
kterm_puthex:
    ; eax is 32 bits, so 8 hex digits
    pushad

    mov cl, (8*4)

.do:
    sub cl, 4 ; cl is the counter

    mov ebx, 0xf ; ebx is the mask
    shl ebx, cl
    mov edx, eax ; edx is temp eax
    and edx, ebx
    shr edx, cl

    add edx, hexdigits
    push eax
    mov byte al, [edx]
    call kterm_putchar
    pop eax

    cmp cl, 0
    jne .do

    popad
    ret

; prints the character `al` to vga video memory
; doesn't take into account ttys or anything, they're
; not even implemented yet lol
; also moves the cursor
kterm_putchar:
    pushad

    cmp al, 10
    je .nl

    cmp byte [kterm_col], SCREEN_COLS
    jge .nlp
.print:
    mov ebx, [kterm_row]
    imul ebx, 80
    add ebx, [kterm_col]
    shl ebx, 1
    add ebx, SCREEN_MIN
    mov byte [ebx], al

    mov al, 3
    call kterm_movecursor
    jmp .end
.nlp: ; newline and print
    mov bl, al
    mov al, 1
    call kterm_movecursor
    mov al, bl
    mov byte [kterm_col], 0
    jmp .print
.nl: ; newline
    mov bl, al
    mov al, 1
    call kterm_movecursor
    mov al, bl
    mov byte [kterm_col], 0
.end:
    popad
    ret

; `al` = direction
; 0 = up
; 1 = down
; 2 = left
; 3 = right
kterm_movecursor:
    pushad

    cmp al, 0
    je .up
    cmp al, 1
    je .down
    cmp al, 2
    je .left
    cmp al, 3
    je .right
.up:
    dec byte [kterm_row]
    jmp .end
.down:
    cmp byte [kterm_row], (SCREEN_ROWS-1)
    jne .s1
    call kterm_scrolldown
    jmp .end
.s1:
    inc byte [kterm_row]
    jmp .end
.left:
    dec byte [kterm_col]
    jmp .end
.right:
    inc byte [kterm_col]
    jmp .end
.end:
    
    popad
    ret

kterm_scrolldown:
    pushad

    mov eax, (SCREEN_MIN-2)
.loop:
    add eax, 2
    cmp eax, SCREEN_MAX
    jg .end
    cmp eax, (SCREEN_MAX - SCREEN_COLS)
    jg .clear 
    mov byte bl, [eax + (SCREEN_COLS*2)]
    mov byte [eax], bl
    jmp .loop
.clear:
    mov byte [eax], 0
    jmp .loop
.end:
    mov byte [kterm_col], 0
    popad
    ret