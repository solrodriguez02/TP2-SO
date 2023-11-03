section .text 

GLOBAL enterRegion
GLOBAL leaveRegion

enterRegion:
    push rax 
tryToUnblock:    
    mov al, 1
    xchg byte [lock], al  
    cmp al,0
    jne continue
    ; block 
    jmp tryToUnblock 
continue:
    pop rax 
    ret

leaveRegion:
    mov byte [lock], 0
    ret

section .bss
lock resb 1