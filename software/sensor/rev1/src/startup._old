        .module startup
        .area   CODE

        .globl  _main
        .globl  _reset_entry

        .globl  s_INITIALIZER
        .globl  s_INITIALIZED
        .globl  l_INITIALIZED

_reset_entry:
        ; Set SP (adjust if needed)
        ldw     x,#0x07FF
        ldw     sp,x

        ; ---- Copy INITIALIZED bytes from INITIALIZER -> INITIALIZED
        ldw     x,#s_INITIALIZED      ; dst
        ldw     y,#s_INITIALIZER      ; src

        ; l_INITIALIZED is 16-bit; for your case it's 0x0008, so we use low byte as count
        ld      a,l_INITIALIZED+1     ; low byte (asxxxx symbol+1 for LSB)
        jreq    data_done

data_loop:
        ld      a,(y)
        ld      (x),a
        incw    y
        incw    x
        dec     a
        jrne    data_loop

data_done:
        call    _main
hang:
        jra     hang
