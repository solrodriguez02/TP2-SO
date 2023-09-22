GLOBAL invalidOperation
GLOBAL loopRegisters

section .text

invalidOperation:
    push rbp
    mov rbp, rsp
    
    mov cr6, rax

    leave
    ret

loopRegisters:
push rbp
mov rbp, rsp

mov r12, 250000000
.loop:
mov rax, 33h
mov rbx, 33h
mov rcx, 33h
mov rdx, 33h
mov rdi, 33h
mov rsi, 33h
mov r8, 33h
mov r9, 33h
mov r10, 33h
mov r11, 33h
mov r13, 33h
mov r14, 33h
mov r15, 33h
dec r12
jnz .loop

mov rsp, rbp
pop rbp
ret
