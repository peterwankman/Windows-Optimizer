; We need to generate 32-bit code
bits 32

; From here we can do whatever we want
; as long as we start with code and
; exit at the end of the source, because
; after the inserted code a jmp to the
; original entry point will be added.

section .text								; This is completely
global _start								; unneccessary.

_start:										; But, whatever.

	jmp end									; Jump over the message.
	msg db 'Brought to you by eBaumsWorld.'	; We need to get credit.

end: