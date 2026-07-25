BITS 32

section .multiboot
align 8
    
header_start:
    ;multiboot 2
    dd 0xe85250d6
    ; 0=i386 proc mode
    dd 0
    ; header length
    dd header_end - header_start
    ;checksum multiboot 2 + 0=i386 proc mode + length = 0 simple enough
    dd -(0xe85250d6 + 0 + (header_end - header_start))

    ;you can add more tags here IF YOU KNOW WHAT THE FUCK YOU DOIN BOII but you can like add for framebuufer wait shit i am alredy gonna do that... uh never mind mb 
    dw 0
    dw 0
    dd 8
header_end:


section .text
global _start

_start:
; check for uhm if correct something will probbably fail but whatever
    cmp eax, 0x36d76289
    jne .not_correct

    ;if i recall correctly grub (the shit ill use) sets up flat 32-bit proc mode segment so ill rely on grub so i dont need a temporary GDT

    ;set up stack (grows downwords)
    mov esp, stack_top

    ;clear the bss section
    extern __bss_start, __bss_end
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    push ebx
    push eax

    extern kmain
    call kmain

    ;if main ever retuns halt the cpu
    cli
.hang:
    hlt 
    jmp .hang

.not_correct:
    ;if it didnt boot with multi
    cli
.hang2:
    hlt
    jmp .hang2


section .bss
align 16
stack_bottom:
    resb 16384  ;its 16 kb
stack_top: