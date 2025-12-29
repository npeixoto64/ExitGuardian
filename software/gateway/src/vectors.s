; STM8 vector table (4 bytes each):
;   0x82 + 24-bit address
; Table lives at 0x8000 (flash start / IVT base).

        .module vectors

        .globl _reset_entry
        .globl _default_isr

; Default ISR: return-from-interrupt
        .area CODE
_default_isr:
        iret

; Vector table: ABS area at 0x8000
; STM8 vectors: 0x82 + 24-bit address (PCE, PCH, PCL). :contentReference[oaicite:3]{index=3}
        .area VECTORS (ABS)
        .org 0x8000

        .macro VEC handler
        .db 0x82
        .db >handler
        .dw handler
        .endm

        ; 32 entries: RESET, TRAP, NMI, and up to 29 IRQs. :contentReference[oaicite:4]{index=4}
        VEC _reset_entry     ; RESET
        VEC _default_isr     ; TRAP
        VEC _default_isr     ; NMI
        VEC _default_isr     ; IRQ0
        VEC _default_isr     ; IRQ1
        VEC _default_isr     ; IRQ2
        VEC _default_isr     ; IRQ3
        VEC _default_isr     ; IRQ4
        VEC _default_isr     ; IRQ5
        VEC _default_isr     ; IRQ6
        VEC _default_isr     ; IRQ7
        VEC _default_isr     ; IRQ8
        VEC _default_isr     ; IRQ9
        VEC _default_isr     ; IRQ10
        VEC _default_isr     ; IRQ11
        VEC _default_isr     ; IRQ12
        VEC _default_isr     ; IRQ13
        VEC _default_isr     ; IRQ14
        VEC _default_isr     ; IRQ15
        VEC _default_isr     ; IRQ16
        VEC _default_isr     ; IRQ17
        VEC _default_isr     ; IRQ18
        VEC _default_isr     ; IRQ19
        VEC _default_isr     ; IRQ20
        VEC _default_isr     ; IRQ21
        VEC _default_isr     ; IRQ22
        VEC _default_isr     ; IRQ23
        VEC _default_isr     ; IRQ24
        VEC _default_isr     ; IRQ25
        VEC _default_isr     ; IRQ26
        VEC _default_isr     ; IRQ27
        VEC _default_isr     ; IRQ28
