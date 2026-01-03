                                      1 ;--------------------------------------------------------
                                      2 ; File Created by SDCC : free open source ANSI-C Compiler
                                      3 ; Version 4.2.0 #13081 (Linux)
                                      4 ;--------------------------------------------------------
                                      5 	.module CMakeCCompilerId
                                      6 	.optsdcc -mstm8
                                      7 	
                                      8 ;--------------------------------------------------------
                                      9 ; Public variables in this module
                                     10 ;--------------------------------------------------------
                                     11 	.globl _info_version
                                     12 	.globl _main
                                     13 	.globl _info_language_extensions_default
                                     14 	.globl _info_language_standard_default
                                     15 	.globl _info_arch
                                     16 	.globl _info_platform
                                     17 	.globl _info_compiler
                                     18 ;--------------------------------------------------------
                                     19 ; ram data
                                     20 ;--------------------------------------------------------
                                     21 	.area DATA
                                     22 ;--------------------------------------------------------
                                     23 ; ram data
                                     24 ;--------------------------------------------------------
                                     25 	.area INITIALIZED
      000001                         26 _info_compiler::
      000001                         27 	.ds 2
      000003                         28 _info_platform::
      000003                         29 	.ds 2
      000005                         30 _info_arch::
      000005                         31 	.ds 2
      000007                         32 _info_language_standard_default::
      000007                         33 	.ds 2
      000009                         34 _info_language_extensions_default::
      000009                         35 	.ds 2
                                     36 ;--------------------------------------------------------
                                     37 ; Stack segment in internal ram
                                     38 ;--------------------------------------------------------
                                     39 	.area	SSEG
      00000B                         40 __start__stack:
      00000B                         41 	.ds	1
                                     42 
                                     43 ;--------------------------------------------------------
                                     44 ; absolute external ram data
                                     45 ;--------------------------------------------------------
                                     46 	.area DABS (ABS)
                                     47 
                                     48 ; default segment ordering for linker
                                     49 	.area HOME
                                     50 	.area GSINIT
                                     51 	.area GSFINAL
                                     52 	.area CONST
                                     53 	.area INITIALIZER
                                     54 	.area CODE
                                     55 
                                     56 ;--------------------------------------------------------
                                     57 ; interrupt vector
                                     58 ;--------------------------------------------------------
                                     59 	.area HOME
      008000                         60 __interrupt_vect:
      008000 82 00 80 07             61 	int s_GSINIT ; reset
                                     62 ;--------------------------------------------------------
                                     63 ; global & static initialisations
                                     64 ;--------------------------------------------------------
                                     65 	.area HOME
                                     66 	.area GSINIT
                                     67 	.area GSFINAL
                                     68 	.area GSINIT
      008007                         69 __sdcc_init_data:
                                     70 ; stm8_genXINIT() start
      008007 AE 00 00         [ 2]   71 	ldw x, #l_DATA
      00800A 27 07            [ 1]   72 	jreq	00002$
      00800C                         73 00001$:
      00800C 72 4F 00 00      [ 1]   74 	clr (s_DATA - 1, x)
      008010 5A               [ 2]   75 	decw x
      008011 26 F9            [ 1]   76 	jrne	00001$
      008013                         77 00002$:
      008013 AE 00 0A         [ 2]   78 	ldw	x, #l_INITIALIZER
      008016 27 09            [ 1]   79 	jreq	00004$
      008018                         80 00003$:
      008018 D6 80 BC         [ 1]   81 	ld	a, (s_INITIALIZER - 1, x)
      00801B D7 00 00         [ 1]   82 	ld	(s_INITIALIZED - 1, x), a
      00801E 5A               [ 2]   83 	decw	x
      00801F 26 F7            [ 1]   84 	jrne	00003$
      008021                         85 00004$:
                                     86 ; stm8_genXINIT() end
                                     87 	.area GSFINAL
      008021 CC 80 04         [ 2]   88 	jp	__sdcc_program_startup
                                     89 ;--------------------------------------------------------
                                     90 ; Home
                                     91 ;--------------------------------------------------------
                                     92 	.area HOME
                                     93 	.area HOME
      008004                         94 __sdcc_program_startup:
      008004 CC 80 C7         [ 2]   95 	jp	_main
                                     96 ;	return from main will return to caller
                                     97 ;--------------------------------------------------------
                                     98 ; code
                                     99 ;--------------------------------------------------------
                                    100 	.area CODE
                                    101 ;	CMakeCCompilerId.c: 853: int main(int argc, char* argv[])
                                    102 ;	-----------------------------------------
                                    103 ;	 function main
                                    104 ;	-----------------------------------------
      0080C7                        105 _main:
      0080C7 52 06            [ 2]  106 	sub	sp, #6
      0080C9 1F 05            [ 2]  107 	ldw	(0x05, sp), x
                                    108 ;	CMakeCCompilerId.c: 857: require += info_compiler[argc];
      0080CB CE 00 01         [ 2]  109 	ldw	x, _info_compiler+0
      0080CE 72 FB 05         [ 2]  110 	addw	x, (0x05, sp)
      0080D1 F6               [ 1]  111 	ld	a, (x)
      0080D2 6B 04            [ 1]  112 	ld	(0x04, sp), a
      0080D4 0F 03            [ 1]  113 	clr	(0x03, sp)
                                    114 ;	CMakeCCompilerId.c: 858: require += info_platform[argc];
      0080D6 CE 00 03         [ 2]  115 	ldw	x, _info_platform+0
      0080D9 72 FB 05         [ 2]  116 	addw	x, (0x05, sp)
      0080DC F6               [ 1]  117 	ld	a, (x)
      0080DD 5F               [ 1]  118 	clrw	x
      0080DE 97               [ 1]  119 	ld	xl, a
      0080DF 72 FB 03         [ 2]  120 	addw	x, (0x03, sp)
      0080E2 1F 01            [ 2]  121 	ldw	(0x01, sp), x
                                    122 ;	CMakeCCompilerId.c: 859: require += info_arch[argc];
      0080E4 CE 00 05         [ 2]  123 	ldw	x, _info_arch+0
      0080E7 72 FB 05         [ 2]  124 	addw	x, (0x05, sp)
      0080EA F6               [ 1]  125 	ld	a, (x)
      0080EB 5F               [ 1]  126 	clrw	x
      0080EC 97               [ 1]  127 	ld	xl, a
      0080ED 72 FB 01         [ 2]  128 	addw	x, (0x01, sp)
      0080F0 1F 03            [ 2]  129 	ldw	(0x03, sp), x
                                    130 ;	CMakeCCompilerId.c: 861: require += info_version[argc];
      0080F2 1E 05            [ 2]  131 	ldw	x, (0x05, sp)
      0080F4 D6 80 24         [ 1]  132 	ld	a, (_info_version+0, x)
      0080F7 5F               [ 1]  133 	clrw	x
      0080F8 97               [ 1]  134 	ld	xl, a
      0080F9 72 FB 03         [ 2]  135 	addw	x, (0x03, sp)
      0080FC 1F 01            [ 2]  136 	ldw	(0x01, sp), x
                                    137 ;	CMakeCCompilerId.c: 875: require += info_language_standard_default[argc];
      0080FE CE 00 07         [ 2]  138 	ldw	x, _info_language_standard_default+0
      008101 72 FB 05         [ 2]  139 	addw	x, (0x05, sp)
      008104 F6               [ 1]  140 	ld	a, (x)
      008105 5F               [ 1]  141 	clrw	x
      008106 97               [ 1]  142 	ld	xl, a
      008107 72 FB 01         [ 2]  143 	addw	x, (0x01, sp)
      00810A 1F 03            [ 2]  144 	ldw	(0x03, sp), x
                                    145 ;	CMakeCCompilerId.c: 876: require += info_language_extensions_default[argc];
      00810C CE 00 09         [ 2]  146 	ldw	x, _info_language_extensions_default+0
      00810F 72 FB 05         [ 2]  147 	addw	x, (0x05, sp)
      008112 F6               [ 1]  148 	ld	a, (x)
      008113 5F               [ 1]  149 	clrw	x
      008114 97               [ 1]  150 	ld	xl, a
      008115 72 FB 03         [ 2]  151 	addw	x, (0x03, sp)
                                    152 ;	CMakeCCompilerId.c: 878: return require;
                                    153 ;	CMakeCCompilerId.c: 879: }
      008118 5B 06            [ 2]  154 	addw	sp, #6
      00811A 90 85            [ 2]  155 	popw	y
      00811C 5B 02            [ 2]  156 	addw	sp, #2
      00811E 90 FC            [ 2]  157 	jp	(y)
                                    158 	.area CODE
                                    159 	.area CONST
      008024                        160 _info_version:
      008024 49                     161 	.db #0x49	; 73	'I'
      008025 4E                     162 	.db #0x4e	; 78	'N'
      008026 46                     163 	.db #0x46	; 70	'F'
      008027 4F                     164 	.db #0x4f	; 79	'O'
      008028 3A                     165 	.db #0x3a	; 58
      008029 63                     166 	.db #0x63	; 99	'c'
      00802A 6F                     167 	.db #0x6f	; 111	'o'
      00802B 6D                     168 	.db #0x6d	; 109	'm'
      00802C 70                     169 	.db #0x70	; 112	'p'
      00802D 69                     170 	.db #0x69	; 105	'i'
      00802E 6C                     171 	.db #0x6c	; 108	'l'
      00802F 65                     172 	.db #0x65	; 101	'e'
      008030 72                     173 	.db #0x72	; 114	'r'
      008031 5F                     174 	.db #0x5f	; 95
      008032 76                     175 	.db #0x76	; 118	'v'
      008033 65                     176 	.db #0x65	; 101	'e'
      008034 72                     177 	.db #0x72	; 114	'r'
      008035 73                     178 	.db #0x73	; 115	's'
      008036 69                     179 	.db #0x69	; 105	'i'
      008037 6F                     180 	.db #0x6f	; 111	'o'
      008038 6E                     181 	.db #0x6e	; 110	'n'
      008039 5B                     182 	.db #0x5b	; 91
      00803A 30                     183 	.db #0x30	; 48	'0'
      00803B 30                     184 	.db #0x30	; 48	'0'
      00803C 30                     185 	.db #0x30	; 48	'0'
      00803D 30                     186 	.db #0x30	; 48	'0'
      00803E 30                     187 	.db #0x30	; 48	'0'
      00803F 30                     188 	.db #0x30	; 48	'0'
      008040 30                     189 	.db #0x30	; 48	'0'
      008041 34                     190 	.db #0x34	; 52	'4'
      008042 2E                     191 	.db #0x2e	; 46
      008043 30                     192 	.db #0x30	; 48	'0'
      008044 30                     193 	.db #0x30	; 48	'0'
      008045 30                     194 	.db #0x30	; 48	'0'
      008046 30                     195 	.db #0x30	; 48	'0'
      008047 30                     196 	.db #0x30	; 48	'0'
      008048 30                     197 	.db #0x30	; 48	'0'
      008049 30                     198 	.db #0x30	; 48	'0'
      00804A 32                     199 	.db #0x32	; 50	'2'
      00804B 2E                     200 	.db #0x2e	; 46
      00804C 30                     201 	.db #0x30	; 48	'0'
      00804D 30                     202 	.db #0x30	; 48	'0'
      00804E 30                     203 	.db #0x30	; 48	'0'
      00804F 30                     204 	.db #0x30	; 48	'0'
      008050 30                     205 	.db #0x30	; 48	'0'
      008051 30                     206 	.db #0x30	; 48	'0'
      008052 30                     207 	.db #0x30	; 48	'0'
      008053 30                     208 	.db #0x30	; 48	'0'
      008054 5D                     209 	.db #0x5d	; 93
      008055 00                     210 	.db #0x00	; 0
                                    211 	.area CONST
      008056                        212 ___str_0:
      008056 49 4E 46 4F 3A 63 6F   213 	.ascii "INFO:compiler[SDCC]"
             6D 70 69 6C 65 72 5B
             53 44 43 43 5D
      008069 00                     214 	.db 0x00
                                    215 	.area CODE
                                    216 	.area CONST
      00806A                        217 ___str_1:
      00806A 49 4E 46 4F 3A 70 6C   218 	.ascii "INFO:platform[]"
             61 74 66 6F 72 6D 5B
             5D
      008079 00                     219 	.db 0x00
                                    220 	.area CODE
                                    221 	.area CONST
      00807A                        222 ___str_2:
      00807A 49 4E 46 4F 3A 61 72   223 	.ascii "INFO:arch[]"
             63 68 5B 5D
      008085 00                     224 	.db 0x00
                                    225 	.area CODE
                                    226 	.area CONST
      008086                        227 ___str_3:
      008086 49 4E 46 4F 3A 73 74   228 	.ascii "INFO:standard_default[11]"
             61 6E 64 61 72 64 5F
             64 65 66 61 75 6C 74
             5B 31 31 5D
      00809F 00                     229 	.db 0x00
                                    230 	.area CODE
                                    231 	.area CONST
      0080A0                        232 ___str_4:
      0080A0 49 4E 46 4F 3A 65 78   233 	.ascii "INFO:extensions_default[OFF]"
             74 65 6E 73 69 6F 6E
             73 5F 64 65 66 61 75
             6C 74 5B 4F 46 46 5D
      0080BC 00                     234 	.db 0x00
                                    235 	.area CODE
                                    236 	.area INITIALIZER
      0080BD                        237 __xinit__info_compiler:
      0080BD 80 56                  238 	.dw ___str_0
      0080BF                        239 __xinit__info_platform:
      0080BF 80 6A                  240 	.dw ___str_1
      0080C1                        241 __xinit__info_arch:
      0080C1 80 7A                  242 	.dw ___str_2
      0080C3                        243 __xinit__info_language_standard_default:
      0080C3 80 86                  244 	.dw ___str_3
      0080C5                        245 __xinit__info_language_extensions_default:
      0080C5 80 A0                  246 	.dw ___str_4
                                    247 	.area CABS (ABS)
