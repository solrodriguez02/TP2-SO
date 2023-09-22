GLOBAL syscall_write
GLOBAL syscall_read
GLOBAL syscall_nextLine
GLOBAL syscall_wait
GLOBAL syscall_time
GLOBAL syscall_beep
GLOBAL syscall_drawCircle
GLOBAL syscall_drawRectangle
GLOBAL syscall_loadScreen
GLOBAL syscall_enableDoubleBuffer
GLOBAL syscall_getKeyboardState
GLOBAL syscall_writeAt
GLOBAL syscall_getRegisters

section .text

syscall_read:
    pushf
    mov rdi,0      ; syscall for read
    int 80h
    popf
    ret

;se pasa primero la cadena, despues el largo
syscall_write:
    pushf
    mov rcx, rdx   ; pasaje de parametros
	mov rdx, rsi   ; el segundo argumento que se pasa (en rsi) es el 
	mov rsi, rdi   ; en rsi hay que pasar la cadena de cracteres a imprimirse, la cual es el primer parametro que se encuentra en rdi
    mov rdi,1      ; syscall for write
    int 80h
    popf
    ret

syscall_nextLine:
    pushf
    mov rdi,2      ; syscall for nextLine
    int 80h
    popf
    ret

syscall_wait:
	pushf
	mov rsi,rdi 	; # de segundos
	mov rdi,3
	int 80h
	popf
	ret

syscall_time:
	pushf
	mov rsi, rdi
	mov rdi, 4 		; syscall for time
	int 80h
	popf
	ret

syscall_getRegisters:
	pushf
	mov rdi,5	    ; syscall for getRegisters 
	int 80h
	popf
	ret

syscall_beep:
	pushf
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 6
	int 80h
	popf
	ret

syscall_drawCircle:
	pushf
	mov r8, rcx		; pasaje de parametros
	mov rcx, rdx  
	mov rdx, rsi 
	mov rsi, rdi
	mov rdi, 7 		; syscall for drawCircle
	int 80h
	popf
	ret

syscall_drawRectangle:
	pushf
	mov r9, r8		 ; pasaje de parametros
	mov r8, rcx	
	mov rcx, rdx  
	mov rdx, rsi 
	mov rsi, rdi
	mov rdi, 8 		 ; syscall for drawRectangle
	int 80h
	popf
	ret

syscall_loadScreen:
	pushf
	mov rdi, 9 	     ; syscall for cleanScreen
	int 80h
	popf
	ret

syscall_enableDoubleBuffer:
	pushf 
	mov rsi, rdi
	mov rdi, 10 	 ; syscall for pixel
	int 80h
	popf
	ret

syscall_getKeyboardState:
	pushf 
	mov rsi, rdi
	mov rdi, 11 	  ; syscall for getKeyboardState
	int 80h
	popf
	ret

syscall_writeAt:
    pushf
	mov r9, r8		  ; pasaje de parametros
	mov r8, rcx
    mov rcx, rdx   
	mov rdx, rsi	
	mov rsi, rdi	
    mov rdi, 12       ; syscall for writeAt
    int 80h
    popf
    ret
	