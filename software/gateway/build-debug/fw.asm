;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.2.0 #13081 (Linux)
;--------------------------------------------------------
	.module main
	.optsdcc -mstm8
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _zero
	.globl _counter
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area INITIALIZED
G$counter$0_0$0==.
_counter::
	.ds 4
G$zero$0_0$0==.
_zero::
	.ds 4
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
	Fmain$delay$0$0 ==.
	C$main.c$14$0_0$3 ==.
;	/workspace/src/main.c: 14: static void delay(void)
; genLabel
;	-----------------------------------------
;	 function delay
;	-----------------------------------------
;	Register assignment might be sub-optimal.
;	Stack space usage: 4 bytes.
_delay:
	sub	sp, #4
	C$main.c$17$2_0$3 ==.
;	/workspace/src/main.c: 17: for (i = 0; i < 20000UL; i++) {}
; genAssign
	clrw	x
	ldw	(0x03, sp), x
	ldw	(0x01, sp), x
; genLabel
00103$:
; genCmp
; genCmpTnz
	ldw	x, (0x03, sp)
	cpw	x, #0x4e20
	ld	a, (0x02, sp)
	sbc	a, #0x00
	ld	a, (0x01, sp)
	sbc	a, #0x00
; peephole j5 changed absolute to relative unconditional jump.
	jrnc	00105$
; peephole j6 removed jra by using inverse jump logic
; peephole j30 removed unused label 00117$.
; skipping generated iCode
; genAssign
	ldw	x, (0x03, sp)
	ldw	y, (0x01, sp)
; genPlus
	incw	x
	jrne	00118$
	incw	y
00118$:
; genAssign
	ldw	(0x03, sp), x
	ldw	(0x01, sp), y
; genGoto
	jra	00103$
; peephole j5 changed absolute to relative unconditional jump.
; genLabel
00105$:
	C$main.c$18$2_0$3 ==.
;	/workspace/src/main.c: 18: }
; genEndFunction
	addw	sp, #4
	C$main.c$18$2_0$3 ==.
	XFmain$delay$0$0 ==.
	ret
	G$main$0$0 ==.
	C$main.c$20$2_0$6 ==.
;	/workspace/src/main.c: 20: int main(void)
; genLabel
;	-----------------------------------------
;	 function main
;	-----------------------------------------
;	Register assignment is optimal.
;	Stack space usage: 0 bytes.
_main:
	C$main.c$22$1_0$6 ==.
;	/workspace/src/main.c: 22: counter = 5;
; genAssign
	ldw	x, #0x0005
	ldw	_counter+2, x
	clrw	x
	ldw	_counter+0, x
	C$main.c$23$1_0$6 ==.
;	/workspace/src/main.c: 23: zero = 0;
; genAssign
	clrw	x
	ldw	_zero+2, x
	ldw	_zero+0, x
	C$main.c$25$1_0$6 ==.
;	/workspace/src/main.c: 25: PA_DDR |= PA2;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5002, #2
; peephole 202x replaced 'or' by 'bset' ('0x5002').
	C$main.c$26$1_0$6 ==.
;	/workspace/src/main.c: 26: PA_CR1 |= PA2;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5003, #2
; peephole 202x replaced 'or' by 'bset' ('0x5003').
	C$main.c$27$1_0$6 ==.
;	/workspace/src/main.c: 27: PA_CR2 &= (uint8_t)~PA2;
; genPointerGet
; genAnd
; genPointerSet
	bres	0x5004, #2
; peephole 204x replaced 'and' by 'bres' ('0x5004').
	C$main.c$29$1_0$6 ==.
;	/workspace/src/main.c: 29: PA_DDR |= PA3;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5002, #3
; peephole 202x replaced 'or' by 'bset' ('0x5002').
	C$main.c$30$1_0$6 ==.
;	/workspace/src/main.c: 30: PA_CR1 |= PA3;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5003, #3
; peephole 202x replaced 'or' by 'bset' ('0x5003').
	C$main.c$31$1_0$6 ==.
;	/workspace/src/main.c: 31: PA_CR2 &= (uint8_t)~PA3;
; genPointerGet
; genAnd
; genPointerSet
	bres	0x5004, #3
; peephole 204x replaced 'and' by 'bres' ('0x5004').
	C$main.c$33$1_0$6 ==.
;	/workspace/src/main.c: 33: PA_DDR |= PA4;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5002, #4
; peephole 202x replaced 'or' by 'bset' ('0x5002').
	C$main.c$34$1_0$6 ==.
;	/workspace/src/main.c: 34: PA_CR1 |= PA4;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5003, #4
; peephole 202x replaced 'or' by 'bset' ('0x5003').
	C$main.c$35$1_0$6 ==.
;	/workspace/src/main.c: 35: PA_CR2 &= (uint8_t)~PA4;
; genPointerGet
; genAnd
; genPointerSet
	bres	0x5004, #4
; peephole 204x replaced 'and' by 'bres' ('0x5004').
	C$main.c$37$1_0$6 ==.
;	/workspace/src/main.c: 37: if (counter == 5)
; genCmpEQorNE
	ldw	x, _counter+2
	cpw	x, #0x0005
	jrne	00102$
; peephole j22 jumped to 00102$ directly instead of via 00126$.
	ldw	x, _counter+0
; peephole j5 changed absolute to relative unconditional jump.
; peephole j10 removed jra by using inverse jump logic
; peephole j30 removed unused label 00126$.
; peephole j5 changed absolute to relative unconditional jump.
	jrne	00102$
; peephole j7 removed jra by using inverse jump logic
; peephole j30 removed unused label 00127$.
; skipping generated iCode
	C$main.c$38$1_0$6 ==.
;	/workspace/src/main.c: 38: PA_ODR |= PA3;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5000, #3
; peephole 202x replaced 'or' by 'bset' ('0x5000').
; genLabel
00102$:
	C$main.c$40$1_0$6 ==.
;	/workspace/src/main.c: 40: if (zero == 0)
; genIfx
	ldw	x, _zero+2
	jrne	00106$
; peephole j22 jumped to 00106$ directly instead of via 00128$.
	ldw	x, _zero+0
; peephole j30 removed unused label 00128$.
; peephole j5 changed absolute to relative unconditional jump.
	jrne	00106$
; peephole j7 removed jra by using inverse jump logic
; peephole j30 removed unused label 00129$.
	C$main.c$41$1_0$6 ==.
;	/workspace/src/main.c: 41: PA_ODR |= PA4;
; genPointerGet
; genOr
; genPointerSet
	bset	0x5000, #4
; peephole 202x replaced 'or' by 'bset' ('0x5000').
	C$main.c$43$1_0$6 ==.
;	/workspace/src/main.c: 43: while (1) {
; genLabel
00106$:
	C$main.c$44$2_0$7 ==.
;	/workspace/src/main.c: 44: PA_ODR ^= PA2;
; genPointerGet
; genXor
; genPointerSet
	bcpl	0x5000, #2
; peephole 210x replaced 'xor' by 'bcpl' ('0x5000').
	C$main.c$45$2_0$7 ==.
;	/workspace/src/main.c: 45: delay();
; genCall
	call	_delay
; genGoto
	jra	00106$
; peephole j5 changed absolute to relative unconditional jump.
; genLabel
; peephole j30 removed unused label 00108$.
	C$main.c$47$1_0$6 ==.
;	/workspace/src/main.c: 47: }
; genEndFunction
	C$main.c$47$1_0$6 ==.
	XG$main$0$0 ==.
	ret
	.area CODE
	.area CONST
	.area INITIALIZER
Fmain$__xinit_counter$0_0$0 == .
__xinit__counter:
	.byte #0x00, #0x00, #0x00, #0x05	; 5
Fmain$__xinit_zero$0_0$0 == .
__xinit__zero:
	.byte #0x00, #0x00, #0x00, #0x00	; 0
	.area CABS (ABS)
