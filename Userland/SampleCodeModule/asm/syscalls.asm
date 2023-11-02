GLOBAL syscall_write
GLOBAL syscall_read
GLOBAL syscall_nextLine
GLOBAL syscall_wait
GLOBAL syscall_time
GLOBAL syscall_beep
GLOBAL syscall_loadScreen
GLOBAL syscall_enableDoubleBuffer
GLOBAL syscall_writeAt
GLOBAL syscall_malloc
GLOBAL syscall_free
GLOBAL syscall_getpid
GLOBAL syscall_getstatus
GLOBAL syscall_kill
GLOBAL syscall_execve
GLOBAL syscall_block
GLOBAL syscall_update_priority
GLOBAL syscall_get_priority
GLOBAL syscall_yield
GLOBAL syscall_waitChildren
GLOBAL syscall_createPipe
GLOBAL syscall_getAllProcessInfo

section .text

syscall_read:
    pushf
    mov rcx, rdx   ; count
	mov rdx, rsi   ; &placeholder
	mov rsi, rdi   ; fd
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

syscall_beep:
	pushf
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 6
	int 80h
	popf
	ret

syscall_getAllProcessInfo:
	pushf
	mov rsi, rdi
	mov rdi, 8
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

syscall_malloc:
	pushf 
	mov rsi, rdi
	mov rdi, 13 	 ; syscall for malloc
	int 80h
	popf
	ret

syscall_free:
	pushf 
	mov rsi, rdi
	mov rdi, 14 	 ; syscall for free
	int 80h
	popf
	ret

syscall_getpid:
	pushf	
	mov rdi, 15
	int 80h
	popf
	ret

syscall_getstatus:
	pushf
	mov rsi, rdi
	mov rdi, 16
	int 80h
	popf
	ret

syscall_kill:
	pushf
	mov rsi, rdi
	mov rdi, 17
	int 80h
	popf
	ret

syscall_execve:
	pushf
	mov r8, rcx
    mov rcx, rdx   
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 18
	int 80h
	popf
	ret

syscall_block:
	pushf
	mov rsi, rdi
	mov rdi, 19
	int 80h
	popf
	ret

syscall_update_priority:
	pushf
	mov rsi, rdi
	mov rdi, 20
	int 80h
	popf
	ret

syscall_get_priority:
	pushf
	mov rsi, rdi
	mov rdi, 21
	int 80h
	popf
	ret

syscall_yield:
	pushf
	mov rdi, 22
	int 80h
	popf
	ret

syscall_waitChildren:
	pushf
	mov rdi, 23
	int 80h
	popf
	ret

syscall_createPipe:
	pushf
	mov rdi, 24
	int 80h
	popf
	ret