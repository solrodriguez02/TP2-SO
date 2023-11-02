GLOBAL enterRegion
GLOBAL leaveRegion

enterRegion:
    mov rax, 1
    mov r16, [lock]
    xchg ax, r16
    mov byte [lock], ax
    ret

leaveRegion:
    mov [lock], 0
    ret


section .bss:
    lock: resb 1