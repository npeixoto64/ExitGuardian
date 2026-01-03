                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 4.2.0 #13081 (Linux)
                                      4 ;--------------------------------------------------------
                                      5 	.module main
                                      6 	.optsdcc -mstm8
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _main
                                     12 	.globl _zero
                                     13 	.globl _counter
                                     14 ;--------------------------------------------------------
                                     15 ; ram data
                                     16 ;--------------------------------------------------------
                                     17 	.area DATA
                                     18 ;--------------------------------------------------------
                                     19 ; ram data
                                     20 ;--------------------------------------------------------
                                     21 	.area INITIALIZED
                           000000    22 G$counter$0_0$0==.
      000000                         23 _counter::
      000000                         24 	.ds 4
                           000004    25 G$zero$0_0$0==.
      000004                         26 _zero::
      000004                         27 	.ds 4
                                     28 ;--------------------------------------------------------
                                     29 ; Stack segment in internal ram
                                     30 ;--------------------------------------------------------
                                     31 	.area	SSEG
      000008                         32 __start__stack:
      000008                         33 	.ds	1
                                     34 
                                     35 ;--------------------------------------------------------
                                     36 ; absolute external ram data
                                     37 ;--------------------------------------------------------
                                     38 	.area DABS (ABS)
                                     39 
                                     40 ; default segment ordering for linker
                                     41 	.area HOME
                                     42 	.area GSINIT
                                     43 	.area GSFINAL
                                     44 	.area CONST
                                     45 	.area INITIALIZER
                                     46 	.area CODE
                                     47 
                                     48 ;--------------------------------------------------------
                                     49 ; interrupt vector
                                     50 ;--------------------------------------------------------
                                     51 	.area HOME
      008000                         52 __interrupt_vect:
      008000 82 00 80 07             53 	int s_GSINIT ; reset
                                     54 ;--------------------------------------------------------
                                     55 ; global & static initialisations
                                     56 ;--------------------------------------------------------
                                     57 	.area HOME
                                     58 	.area GSINIT
                                     59 	.area GSFINAL
                                     60 	.area GSINIT
      008007                         61 __sdcc_init_data:
                                     62 ; stm8_genXINIT() start
      008007 AE 00 00         [ 2]   63 	ldw x, #l_DATA
      00800A 27 07            [ 1]   64 	jreq	00002$
      00800C                         65 00001$:
      00800C 72 4F FF FF      [ 1]   66 	clr (s_DATA - 1, x)
      008010 5A               [ 2]   67 	decw x
      008011 26 F9            [ 1]   68 	jrne	00001$
      008013                         69 00002$:
      008013 AE 00 08         [ 2]   70 	ldw	x, #l_INITIALIZER
      008016 27 09            [ 1]   71 	jreq	00004$
      008018                         72 00003$:
      008018 D6 80 23         [ 1]   73 	ld	a, (s_INITIALIZER - 1, x)
      00801B D7 FF FF         [ 1]   74 	ld	(s_INITIALIZED - 1, x), a
      00801E 5A               [ 2]   75 	decw	x
      00801F 26 F7            [ 1]   76 	jrne	00003$
      008021                         77 00004$:
                                     78 ; stm8_genXINIT() end
                                     79 	.area GSFINAL
      008021 CC 80 04         [ 2]   80 	jp	__sdcc_program_startup
                                     81 ;--------------------------------------------------------
                                     82 ; Home
                                     83 ;--------------------------------------------------------
                                     84 	.area HOME
                                     85 	.area HOME
      008004                         86 __sdcc_program_startup:
      008004 CC 80 54         [ 2]   87 	jp	_main
                                     88 ;	return from main will return to caller
                                     89 ;--------------------------------------------------------
                                     90 ; code
                                     91 ;--------------------------------------------------------
                                     92 	.area CODE
                           000000    93 	Fmain$delay$0$0 ==.
                           000000    94 	C$main.c$14$0_0$3 ==.
                                     95 ;	/workspace/src/main.c: 14: static void delay(void)
                                     96 ; genLabel
                                     97 ;	-----------------------------------------
                                     98 ;	 function delay
                                     99 ;	-----------------------------------------
                                    100 ;	Register assignment might be sub-optimal.
                                    101 ;	Stack space usage: 4 bytes.
      00802C                        102 _delay:
      00802C 52 04            [ 2]  103 	sub	sp, #4
                           000002   104 	C$main.c$17$2_0$3 ==.
                                    105 ;	/workspace/src/main.c: 17: for (i = 0; i < 20000UL; i++) {}
                                    106 ; genAssign
      00802E 5F               [ 1]  107 	clrw	x
      00802F 1F 03            [ 2]  108 	ldw	(0x03, sp), x
      008031 1F 01            [ 2]  109 	ldw	(0x01, sp), x
                                    110 ; genLabel
      008033                        111 00103$:
                                    112 ; genCmp
                                    113 ; genCmpTnz
      008033 1E 03            [ 2]  114 	ldw	x, (0x03, sp)
      008035 A3 4E 20         [ 2]  115 	cpw	x, #0x4e20
      008038 7B 02            [ 1]  116 	ld	a, (0x02, sp)
      00803A A2 00            [ 1]  117 	sbc	a, #0x00
      00803C 7B 01            [ 1]  118 	ld	a, (0x01, sp)
      00803E A2 00            [ 1]  119 	sbc	a, #0x00
                                    120 ; peephole j5 changed absolute to relative unconditional jump.
      008040 24 0F            [ 1]  121 	jrnc	00105$
                                    122 ; peephole j6 removed jra by using inverse jump logic
                                    123 ; peephole j30 removed unused label 00117$.
                                    124 ; skipping generated iCode
                                    125 ; genAssign
      008042 1E 03            [ 2]  126 	ldw	x, (0x03, sp)
      008044 16 01            [ 2]  127 	ldw	y, (0x01, sp)
                                    128 ; genPlus
      008046 5C               [ 1]  129 	incw	x
      008047 26 02            [ 1]  130 	jrne	00118$
      008049 90 5C            [ 1]  131 	incw	y
      00804B                        132 00118$:
                                    133 ; genAssign
      00804B 1F 03            [ 2]  134 	ldw	(0x03, sp), x
      00804D 17 01            [ 2]  135 	ldw	(0x01, sp), y
                                    136 ; genGoto
      00804F 20 E2            [ 2]  137 	jra	00103$
                                    138 ; peephole j5 changed absolute to relative unconditional jump.
                                    139 ; genLabel
      008051                        140 00105$:
                           000025   141 	C$main.c$18$2_0$3 ==.
                                    142 ;	/workspace/src/main.c: 18: }
                                    143 ; genEndFunction
      008051 5B 04            [ 2]  144 	addw	sp, #4
                           000027   145 	C$main.c$18$2_0$3 ==.
                           000027   146 	XFmain$delay$0$0 ==.
      008053 81               [ 4]  147 	ret
                           000028   148 	G$main$0$0 ==.
                           000028   149 	C$main.c$20$2_0$6 ==.
                                    150 ;	/workspace/src/main.c: 20: int main(void)
                                    151 ; genLabel
                                    152 ;	-----------------------------------------
                                    153 ;	 function main
                                    154 ;	-----------------------------------------
                                    155 ;	Register assignment is optimal.
                                    156 ;	Stack space usage: 0 bytes.
      008054                        157 _main:
                           000028   158 	C$main.c$22$1_0$6 ==.
                                    159 ;	/workspace/src/main.c: 22: counter = 5;
                                    160 ; genAssign
      008054 AE 00 05         [ 2]  161 	ldw	x, #0x0005
      008057 CF 00 02         [ 2]  162 	ldw	_counter+2, x
      00805A 5F               [ 1]  163 	clrw	x
      00805B CF 00 00         [ 2]  164 	ldw	_counter+0, x
                           000032   165 	C$main.c$23$1_0$6 ==.
                                    166 ;	/workspace/src/main.c: 23: zero = 0;
                                    167 ; genAssign
      00805E 5F               [ 1]  168 	clrw	x
      00805F CF 00 06         [ 2]  169 	ldw	_zero+2, x
      008062 CF 00 04         [ 2]  170 	ldw	_zero+0, x
                           000039   171 	C$main.c$25$1_0$6 ==.
                                    172 ;	/workspace/src/main.c: 25: PA_DDR |= PA2;
                                    173 ; genPointerGet
                                    174 ; genOr
                                    175 ; genPointerSet
      008065 72 14 50 02      [ 1]  176 	bset	0x5002, #2
                                    177 ; peephole 202x replaced 'or' by 'bset' ('0x5002').
                           00003D   178 	C$main.c$26$1_0$6 ==.
                                    179 ;	/workspace/src/main.c: 26: PA_CR1 |= PA2;
                                    180 ; genPointerGet
                                    181 ; genOr
                                    182 ; genPointerSet
      008069 72 14 50 03      [ 1]  183 	bset	0x5003, #2
                                    184 ; peephole 202x replaced 'or' by 'bset' ('0x5003').
                           000041   185 	C$main.c$27$1_0$6 ==.
                                    186 ;	/workspace/src/main.c: 27: PA_CR2 &= (uint8_t)~PA2;
                                    187 ; genPointerGet
                                    188 ; genAnd
                                    189 ; genPointerSet
      00806D 72 15 50 04      [ 1]  190 	bres	0x5004, #2
                                    191 ; peephole 204x replaced 'and' by 'bres' ('0x5004').
                           000045   192 	C$main.c$29$1_0$6 ==.
                                    193 ;	/workspace/src/main.c: 29: PA_DDR |= PA3;
                                    194 ; genPointerGet
                                    195 ; genOr
                                    196 ; genPointerSet
      008071 72 16 50 02      [ 1]  197 	bset	0x5002, #3
                                    198 ; peephole 202x replaced 'or' by 'bset' ('0x5002').
                           000049   199 	C$main.c$30$1_0$6 ==.
                                    200 ;	/workspace/src/main.c: 30: PA_CR1 |= PA3;
                                    201 ; genPointerGet
                                    202 ; genOr
                                    203 ; genPointerSet
      008075 72 16 50 03      [ 1]  204 	bset	0x5003, #3
                                    205 ; peephole 202x replaced 'or' by 'bset' ('0x5003').
                           00004D   206 	C$main.c$31$1_0$6 ==.
                                    207 ;	/workspace/src/main.c: 31: PA_CR2 &= (uint8_t)~PA3;
                                    208 ; genPointerGet
                                    209 ; genAnd
                                    210 ; genPointerSet
      008079 72 17 50 04      [ 1]  211 	bres	0x5004, #3
                                    212 ; peephole 204x replaced 'and' by 'bres' ('0x5004').
                           000051   213 	C$main.c$33$1_0$6 ==.
                                    214 ;	/workspace/src/main.c: 33: PA_DDR |= PA4;
                                    215 ; genPointerGet
                                    216 ; genOr
                                    217 ; genPointerSet
      00807D 72 18 50 02      [ 1]  218 	bset	0x5002, #4
                                    219 ; peephole 202x replaced 'or' by 'bset' ('0x5002').
                           000055   220 	C$main.c$34$1_0$6 ==.
                                    221 ;	/workspace/src/main.c: 34: PA_CR1 |= PA4;
                                    222 ; genPointerGet
                                    223 ; genOr
                                    224 ; genPointerSet
      008081 72 18 50 03      [ 1]  225 	bset	0x5003, #4
                                    226 ; peephole 202x replaced 'or' by 'bset' ('0x5003').
                           000059   227 	C$main.c$35$1_0$6 ==.
                                    228 ;	/workspace/src/main.c: 35: PA_CR2 &= (uint8_t)~PA4;
                                    229 ; genPointerGet
                                    230 ; genAnd
                                    231 ; genPointerSet
      008085 72 19 50 04      [ 1]  232 	bres	0x5004, #4
                                    233 ; peephole 204x replaced 'and' by 'bres' ('0x5004').
                           00005D   234 	C$main.c$37$1_0$6 ==.
                                    235 ;	/workspace/src/main.c: 37: if (counter == 5)
                                    236 ; genCmpEQorNE
      008089 CE 00 02         [ 2]  237 	ldw	x, _counter+2
      00808C A3 00 05         [ 2]  238 	cpw	x, #0x0005
      00808F 26 09            [ 1]  239 	jrne	00102$
                                    240 ; peephole j22 jumped to 00102$ directly instead of via 00126$.
      008091 CE 00 00         [ 2]  241 	ldw	x, _counter+0
                                    242 ; peephole j5 changed absolute to relative unconditional jump.
                                    243 ; peephole j10 removed jra by using inverse jump logic
                                    244 ; peephole j30 removed unused label 00126$.
                                    245 ; peephole j5 changed absolute to relative unconditional jump.
      008094 26 04            [ 1]  246 	jrne	00102$
                                    247 ; peephole j7 removed jra by using inverse jump logic
                                    248 ; peephole j30 removed unused label 00127$.
                                    249 ; skipping generated iCode
                           00006A   250 	C$main.c$38$1_0$6 ==.
                                    251 ;	/workspace/src/main.c: 38: PA_ODR |= PA3;
                                    252 ; genPointerGet
                                    253 ; genOr
                                    254 ; genPointerSet
      008096 72 16 50 00      [ 1]  255 	bset	0x5000, #3
                                    256 ; peephole 202x replaced 'or' by 'bset' ('0x5000').
                                    257 ; genLabel
      00809A                        258 00102$:
                           00006E   259 	C$main.c$40$1_0$6 ==.
                                    260 ;	/workspace/src/main.c: 40: if (zero == 0)
                                    261 ; genIfx
      00809A CE 00 06         [ 2]  262 	ldw	x, _zero+2
      00809D 26 09            [ 1]  263 	jrne	00106$
                                    264 ; peephole j22 jumped to 00106$ directly instead of via 00128$.
      00809F CE 00 04         [ 2]  265 	ldw	x, _zero+0
                                    266 ; peephole j30 removed unused label 00128$.
                                    267 ; peephole j5 changed absolute to relative unconditional jump.
      0080A2 26 04            [ 1]  268 	jrne	00106$
                                    269 ; peephole j7 removed jra by using inverse jump logic
                                    270 ; peephole j30 removed unused label 00129$.
                           000078   271 	C$main.c$41$1_0$6 ==.
                                    272 ;	/workspace/src/main.c: 41: PA_ODR |= PA4;
                                    273 ; genPointerGet
                                    274 ; genOr
                                    275 ; genPointerSet
      0080A4 72 18 50 00      [ 1]  276 	bset	0x5000, #4
                                    277 ; peephole 202x replaced 'or' by 'bset' ('0x5000').
                           00007C   278 	C$main.c$43$1_0$6 ==.
                                    279 ;	/workspace/src/main.c: 43: while (1) {
                                    280 ; genLabel
      0080A8                        281 00106$:
                           00007C   282 	C$main.c$44$2_0$7 ==.
                                    283 ;	/workspace/src/main.c: 44: PA_ODR ^= PA2;
                                    284 ; genPointerGet
                                    285 ; genXor
                                    286 ; genPointerSet
      0080A8 90 14 50 00      [ 1]  287 	bcpl	0x5000, #2
                                    288 ; peephole 210x replaced 'xor' by 'bcpl' ('0x5000').
                           000080   289 	C$main.c$45$2_0$7 ==.
                                    290 ;	/workspace/src/main.c: 45: delay();
                                    291 ; genCall
      0080AC CD 80 2C         [ 4]  292 	call	_delay
                                    293 ; genGoto
      0080AF 20 F7            [ 2]  294 	jra	00106$
                                    295 ; peephole j5 changed absolute to relative unconditional jump.
                                    296 ; genLabel
                                    297 ; peephole j30 removed unused label 00108$.
                           000085   298 	C$main.c$47$1_0$6 ==.
                                    299 ;	/workspace/src/main.c: 47: }
                                    300 ; genEndFunction
                           000085   301 	C$main.c$47$1_0$6 ==.
                           000085   302 	XG$main$0$0 ==.
      0080B1 81               [ 4]  303 	ret
                                    304 	.area CODE
                                    305 	.area CONST
                                    306 	.area INITIALIZER
                           000000   307 Fmain$__xinit_counter$0_0$0 == .
      008024                        308 __xinit__counter:
      008024 00 00 00 05            309 	.byte #0x00, #0x00, #0x00, #0x05	; 5
                           000004   310 Fmain$__xinit_zero$0_0$0 == .
      008028                        311 __xinit__zero:
      008028 00 00 00 00            312 	.byte #0x00, #0x00, #0x00, #0x00	; 0
                                    313 	.area CABS (ABS)
