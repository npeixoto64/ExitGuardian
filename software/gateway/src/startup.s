; Minimal startup for STM8 + SDCC
; - set stack pointer
; - call main
; - loop forever on return

        .module startup
        .area CODE

        .globl _main
        .globl _reset_entry

; Reset entry point called from vector table
_reset_entry:
        ; STM8L151K4T6 RAM is 0x0000..0x07FF (2 KB) -> top is 0x07FF
        ldw     x, #0x07FF
        ldw     sp, x

        ; If you later use global initialized variables,
        ; add .data copy and .bss clear here.

        call    _main

1$:
        jra     1$
