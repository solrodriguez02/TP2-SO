GLOBAL readScanCode

section .text

readScanCode:
	push rbp
	mov rbp,rsp

	mov rax, 0
	in al, 60h
	
	leave
	ret

