PMM_BLOCKSIZE equ 4096
PMM_BLOCKSIZE_EXP equ 12 ; 2^PMM_BLOCKSIZE_EXP = PMM_BLOCKSIZE

pmm_init:
    mov dword edx, [ebx]

    bt edx, 0 ; flag 0 specifies if mem_lower and mem_upper are valid
    jc .flag0set
    mov eax, msg_noflag0
    call kterm_putstr
    jmp hltlp
.flag0set:
    mov dword ecx, [ebx + 4]
    shl ecx, 10 ; bootloader told me size in KB, i want it in bytes
    mov dword [pmm_memlowersize], ecx ; amount of lower memory
    mov dword ecx, [ebx + 8]
    shl ecx, 10
    mov dword [pmm_memuppersize], ecx ; amount of upper memory

    mov eax, msg_memlower
    call kterm_putstr
    mov eax, [pmm_memlowersize]
    call kterm_puthex
    mov eax, msg_memupper
    call kterm_putstr
    mov eax, [pmm_memuppersize]
    call kterm_puthex

    ; now kernel knows how much memory there is in the system
    ; work out how many blocks needed
    mov eax, [pmm_memuppersize]
    add eax, 0x100000 ; add the 1M representing the lower memory's range
    shr eax, PMM_BLOCKSIZE_EXP
    mov dword [pmm_amountblocks], eax
    shr eax, 3 ; divide by 8, to get amount of bytes needed
    ; create the bitmap to represent all blocks
    mov ecx, pmm_bitmap
    add eax, ecx
    ; all memory from [ecx] to [eax] should be intialised as 0
.bitmaploop:
    mov byte [ecx], 0
    inc ecx
    cmp ecx, eax
    jg .endbitmaploop
    jmp .bitmaploop
.endbitmaploop:
    mov byte [ecx], 'E'
    ; now i can set all blocks which are not available to 1
    ; these are ones which:
    ; - are set as not available in the mmap
    ; - kernel code
    ; - the pmm bitmap
    ; to find the index of a page, given a physical address,
    ; bit shift the address right by PMM_BLOCKSIZE_EXP.

    mov eax, msg_ksize
    call kterm_putstr
    mov eax, (_kernel_end - _kernel_start)
    call kterm_puthex
    mov eax, _kernel_start
    shr eax, PMM_BLOCKSIZE_EXP
    mov ecx, _kernel_end
    shr ecx, PMM_BLOCKSIZE_EXP
.l1:
    cmp eax, ecx
    jg .l1end
    call pmm_set
    inc eax
    jmp .l1
.l1end:

    bt edx, 6 ; check if memory map is present
    jc .flag6set
    mov eax, msg_noflag6
    call kterm_putstr
    jmp hltlp
.flag6set:
    mov dword eax, [ebx + 44]
    mov dword [pmm_mmaplength], eax ; length of mmap (bytes)
    mov dword eax, [ebx + 48]
    mov dword [pmm_mmapaddress], eax ; address of mmap

    push ebx
    mov ebx, eax ; ebx = address of mmap
    mov ecx, eax
    add dword ecx, [pmm_mmaplength]

.readmmapentry:
    mov al, 10
    call kterm_putchar
    mov eax, msg_start
    call kterm_putstr
    mov dword eax, [ebx+4] ; base
    call kterm_puthex
    mov eax, msg_sizeof
    call kterm_putstr
    mov dword eax, [ebx+12] ; length
    call kterm_puthex
    mov dword eax, [ebx+20] ; type
    cmp eax, 0x1
    jne .rma_s1 ; human readable type
    mov eax, msg_available ; available ram
    call kterm_putstr
    jmp .endloop
.rma_s1:
    mov eax, msg_unavailable ; unavailable, could also be acpi or something
    call kterm_putstr ; but i won't be using that memory
.endloop:
    add ebx, [ebx]
    add ebx, 4
    cmp ebx, ecx
    jl .readmmapentry

    pop ebx
    ret

; inputs:
;  eax = bit number
pmm_set:
    pushad
    mov ecx, eax
    shr eax, 3 ; eax = byte number
    and ecx, 0b111 ; ecx = bit number in byte
    mov bl, 0b1
    shl bl, cl
    or byte [eax + pmm_bitmap], bl
    popad
    ret

; inputs:
;  eax = bit number
pmm_unset:
    pushad
    mov ecx, eax
    shr eax, 3
    and ecx, 0b111
    mov bl, 0b1
    shl bl, cl
    not bl
    and byte [eax + pmm_bitmap], bl
    popad
    ret

; inputs:
;  eax = bit number
; outputs:
;  eax = 0 if bit is set else >0
pmm_test:
    push ebx
    mov byte bl, [eax + pmm_bitmap]
    and eax, 0b111
    bt bx, ax
    jc .set
    mov eax, 0
    jmp .end
.set:
    mov eax, 1
.end:
    pop ebx
    ret

; outputs:
;  eax = index of first free page in memory
pmm_firstfree:
    push ebx
    push ecx
    xor eax, eax
.loop1:
    cmp byte [eax + pmm_bitmap], 0xff
    jne .end_loop1
    inc eax
    jmp .loop1
.end_loop1:
    xor ebx, ebx
    mov byte cl, [eax + pmm_bitmap]
.loop2:
    bt cx, bx
    jnc .end_loop2
    inc bx
    jmp .loop2
.end_loop2:
    shl eax, 3 ; bytes -> bits
    add eax, ebx
    pop ecx
    pop ebx
    ret
