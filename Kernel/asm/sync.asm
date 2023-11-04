section .text 

GLOBAL enter_region
GLOBAL leave_region

enter_region:
    mov rax, 1
    xchg eax, dword [rdi]
    ret

leave_region:
    mov byte [rdi], 0
    ret
