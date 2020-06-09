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
    mov dword [pmm_memlowersize], ecx ; amount of lower memory
    mov dword ecx, [ebx + 8]
    mov dword [pmm_memuppersize], ecx ; amount of upper memory

    mov eax, msg_memlower
    call kterm_putstr
    mov eax, [pmm_memlowersize]
    call kterm_puthex
    mov eax, msg_kb
    call kterm_putstr
    mov eax, msg_memupper
    call kterm_putstr
    mov eax, [pmm_memuppersize]
    call kterm_puthex
    mov eax, msg_kb
    call kterm_putstr

    xchg bx, bx
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
    ; now i can set all blocks which are not available to 1
    ; these are ones which:
    ; - are set as not available in the mmap
    ; - kernel code

    bt edx, 6
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
    ret