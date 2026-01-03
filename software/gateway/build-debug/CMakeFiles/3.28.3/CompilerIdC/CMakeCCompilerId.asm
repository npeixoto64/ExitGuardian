;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.2.0 #13081 (Linux)
;--------------------------------------------------------
	.module CMakeCCompilerId
	.optsdcc -mstm8
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _info_version
	.globl _main
	.globl _info_language_extensions_default
	.globl _info_language_standard_default
	.globl _info_arch
	.globl _info_platform
	.globl _info_compiler
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area INITIALIZED
_info_compiler::
	.ds 2
_info_platform::
	.ds 2
_info_arch::
	.ds 2
_info_language_standard_default::
	.ds 2
_info_language_extensions_default::
	.ds 2
;--------------------------------------------------------
; Stack segment in internal ram
;--------------------------------------------------------
	.area	SSEG
__start__stack:
	.ds	1

;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area DABS (ABS)

; default segment ordering for linker
	.area HOME
	.area GSINIT
	.area GSFINAL
	.area CONST
	.area INITIALIZER
	.area CODE

;--------------------------------------------------------
; interrupt vector
;--------------------------------------------------------
	.area HOME
__interrupt_vect:
	int s_GSINIT ; reset
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area HOME
	.area GSINIT
	.area GSFINAL
	.area GSINIT
__sdcc_init_data:
; stm8_genXINIT() start
	ldw x, #l_DATA
	jreq	00002$
00001$:
	clr (s_DATA - 1, x)
	decw x
	jrne	00001$
00002$:
	ldw	x, #l_INITIALIZER
	jreq	00004$
00003$:
	ld	a, (s_INITIALIZER - 1, x)
	ld	(s_INITIALIZED - 1, x), a
	decw	x
	jrne	00003$
00004$:
; stm8_genXINIT() end
	.area GSFINAL
	jp	__sdcc_program_startup
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area HOME
	.area HOME
__sdcc_program_startup:
	jp	_main
;	return from main will return to caller
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area CODE
;	CMakeCCompilerId.c: 853: int main(int argc, char* argv[])
;	-----------------------------------------
;	 function main
;	-----------------------------------------
_main:
	sub	sp, #6
	ldw	(0x05, sp), x
;	CMakeCCompilerId.c: 857: require += info_compiler[argc];
	ldw	x, _info_compiler+0
	addw	x, (0x05, sp)
	ld	a, (x)
	ld	(0x04, sp), a
	clr	(0x03, sp)
;	CMakeCCompilerId.c: 858: require += info_platform[argc];
	ldw	x, _info_platform+0
	addw	x, (0x05, sp)
	ld	a, (x)
	clrw	x
	ld	xl, a
	addw	x, (0x03, sp)
	ldw	(0x01, sp), x
;	CMakeCCompilerId.c: 859: require += info_arch[argc];
	ldw	x, _info_arch+0
	addw	x, (0x05, sp)
	ld	a, (x)
	clrw	x
	ld	xl, a
	addw	x, (0x01, sp)
	ldw	(0x03, sp), x
;	CMakeCCompilerId.c: 861: require += info_version[argc];
	ldw	x, (0x05, sp)
	ld	a, (_info_version+0, x)
	clrw	x
	ld	xl, a
	addw	x, (0x03, sp)
	ldw	(0x01, sp), x
;	CMakeCCompilerId.c: 875: require += info_language_standard_default[argc];
	ldw	x, _info_language_standard_default+0
	addw	x, (0x05, sp)
	ld	a, (x)
	clrw	x
	ld	xl, a
	addw	x, (0x01, sp)
	ldw	(0x03, sp), x
;	CMakeCCompilerId.c: 876: require += info_language_extensions_default[argc];
	ldw	x, _info_language_extensions_default+0
	addw	x, (0x05, sp)
	ld	a, (x)
	clrw	x
	ld	xl, a
	addw	x, (0x03, sp)
;	CMakeCCompilerId.c: 878: return require;
;	CMakeCCompilerId.c: 879: }
	addw	sp, #6
	popw	y
	addw	sp, #2
	jp	(y)
	.area CODE
	.area CONST
_info_version:
	.db #0x49	; 73	'I'
	.db #0x4e	; 78	'N'
	.db #0x46	; 70	'F'
	.db #0x4f	; 79	'O'
	.db #0x3a	; 58
	.db #0x63	; 99	'c'
	.db #0x6f	; 111	'o'
	.db #0x6d	; 109	'm'
	.db #0x70	; 112	'p'
	.db #0x69	; 105	'i'
	.db #0x6c	; 108	'l'
	.db #0x65	; 101	'e'
	.db #0x72	; 114	'r'
	.db #0x5f	; 95
	.db #0x76	; 118	'v'
	.db #0x65	; 101	'e'
	.db #0x72	; 114	'r'
	.db #0x73	; 115	's'
	.db #0x69	; 105	'i'
	.db #0x6f	; 111	'o'
	.db #0x6e	; 110	'n'
	.db #0x5b	; 91
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x34	; 52	'4'
	.db #0x2e	; 46
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x32	; 50	'2'
	.db #0x2e	; 46
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x30	; 48	'0'
	.db #0x5d	; 93
	.db #0x00	; 0
	.area CONST
___str_0:
	.ascii "INFO:compiler[SDCC]"
	.db 0x00
	.area CODE
	.area CONST
___str_1:
	.ascii "INFO:platform[]"
	.db 0x00
	.area CODE
	.area CONST
___str_2:
	.ascii "INFO:arch[]"
	.db 0x00
	.area CODE
	.area CONST
___str_3:
	.ascii "INFO:standard_default[11]"
	.db 0x00
	.area CODE
	.area CONST
___str_4:
	.ascii "INFO:extensions_default[OFF]"
	.db 0x00
	.area CODE
	.area INITIALIZER
__xinit__info_compiler:
	.dw ___str_0
__xinit__info_platform:
	.dw ___str_1
__xinit__info_arch:
	.dw ___str_2
__xinit__info_language_standard_default:
	.dw ___str_3
__xinit__info_language_extensions_default:
	.dw ___str_4
	.area CABS (ABS)
