GLOBAL cpuVendor
GLOBAL getTime
GLOBAL playSound
GLOBAL noSound
GLOBAL getOldFreq
GLOBAL restoreOldFreq

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

; --------------------------------------------------
getTime:
	push rbp
	mov rbp,rsp

	mov al,dil  
	out 70h, al
	in al, 71h
	
	leave 
	ret 
; --------------------------------------------------
playSound:
	push rbp
	mov rbp, rsp

	mov rax, 1193180
	xor rdx, rdx   
	div edi        

	; resultado del cociente de 1193180/freq en rax
	mov rcx, rax

	mov dx, 0x43	; dx = 0x43
	mov al, 0xB6    ; al = 0xb6   
    out dx, al      ; out 0x43, 0xb6   
    mov dx, 0x42    ; dx = 0x42
    mov al, cl ; (uint8_t) a la división y lo guardo en al       
    out dx, al      ; out 0x42,  (uint8_t Div)
	sar rcx, 8
	mov al, cl
	out dx, al

    in al, 0x61  
	mov bl, al 
    or bl, 3
	cmp al, bl
	je .end
	mov al, bl
	out 0x61, al    
    .end: 
	leave
    ret
; --------------------------------------------------
noSound:
	push rbp
	mov rbp, rsp

	in al, 0x61   
    and al, 0xFC                  
    out 0x61, al  

	leave	
    ret
; --------------------------------------------------
getOldFreq:
	push rbp
	mov rbp, rsp

	mov ax, 0    
    in al, 0x42 

	leave
	ret

restoreOldFreq:
	push rbp
	mov rbp, rsp

	mov ax, di
	out 0x42, al
	mov al, ah
	out 0x42, al

	leave
	ret
    