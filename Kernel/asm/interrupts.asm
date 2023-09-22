
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq08Handler

GLOBAL _irq08init

GLOBAL syscallsHandler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

GLOBAL registers_space

EXTERN irqDispatcher
EXTERN syscallsDispatcher
EXTERN exceptionDispatcher
EXTERN getStackBase
EXTERN waitForEnter
EXTERN getAltTouched

section .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro fill_register_space 0
	mov [registers_space + 1*8],rbx
	mov [registers_space + 2*8],rcx
	mov [registers_space + 3*8],rdx
	mov [registers_space + 4*8],rdi
	mov [registers_space + 5*8],rsi
	mov [registers_space + 6*8],rbp
	mov [registers_space + 7*8],rsp
	mov [registers_space + 8*8],r8
	mov [registers_space + 9*8],r9
	mov [registers_space + 10*8],r10
	mov [registers_space + 11*8],r11
	mov [registers_space + 12*8],r12
	mov [registers_space + 13*8],r13
	mov [registers_space + 14*8],r14
	mov [registers_space + 15*8],r15
%endmacro


%macro exceptionHandler 1

	;primero guardo todos los registros en un espacio reservado, para despues poder imprimirlos 
	;como estaban en el momento que se lanzo la excepcion
	mov [registers_space ], rax
	fill_register_space

	;al llamar a la excepcion se pushean al stack el rsp, los flags, el cs y el rip, en ese orden

	mov rax, [rsp] ;el IP es lo ultimo que quedo pusheado en el stack, entonces es a lo que apunta rsp
	mov [registers_space + 16*8], rax ;guardo el instruction pointer en registers_sapce

	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher

	call getStackBase ;me guarda la base del stack en rax
	mov [rsp + 3*8], rax ;piso el stack pointer que estaba pusheado en el stack con el puntero a la base del stack
						;para que al "reiniciar" la shell se reinicie el stack tambien y no siga creciendo
	
	;piso rsp con la direccion de retorno a la que quiero que vaya: el inicio de la shell
	;seria el main en sampleCodeModule, cuya direccion se puede ver en sampleCodeModule.Id (0x400000)

	mov rax, 0x400000
	mov [rsp], rax
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

syscallsHandler:
	pushf
	call syscallsDispatcher
	popf
	iretq


;8254 Timer (Timer Tick)
_irq00Handler:
	pushState

	mov rdi, 0 ; numero de interrupción para el timer (IRQ0)
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

;Keyboard
_irq01Handler:
	pushState

	mov rax, 0
	in al, 60h ;en la direccion 60h esta el scancode de la tecla que haya sido presionada

	cmp al, 38h ;scancode del left Alt: si se presiono alt guarda los registros, sino no
	jne .dontSaveR

	fill_register_space
	mov rax, [rsp + 14*8] ;cuando se hizo el pushState se pushearon 15 registros de 8 bytes, asi que rsp esta apuntando al ultimo que se pusheo y necestio "bajar" 14 lugares para que apunte a rax
	mov [registers_space],rax ;lo guardo en la primera posicion de registers_space

.dontSaveR:
	mov rdi, 1 ; numero de interrupción para el teclado (IRQ1)
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

;RTC
_irq08init:
	push rax
	push rdx
	mov al, 0x8B
	out 70h, al
	in al, 71h
	mov dl, al
	mov al, 0x8B
	out 70h, al
	mov al, dl
	or al, 0x40
	out 71h, al	
	pop rdx
	pop rax
	ret

_irq08Handler:
	pushState
	
	mov rdi, 8 ; numero de interrupción para el RTC (IRQ8)
	call irqDispatcher

	; signal pic EOI - Slave (End of Interrupt)
	mov al, 20h
	out 0xA0, al
	out 0x20, al

	mov al, 0x0C	; actualizo el registro c
	out 70h, al
	in al, 71h

	popState
	iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Operation Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

section .bss
	registers_space resq 17 ;vector de 17 espacios de 8 bytes (cantidad de bytes que necesito para cada registro (16 + el ip)