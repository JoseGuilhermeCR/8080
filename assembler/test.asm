; text
    .org 100h
    lxi sp, 0x07FF
    lxi h, msg
loop:
    mov a, m ; Move from memory at HL to accumulator.
    out 1    ; write accumulator to output port 1.
    INX H    ; increment HL.
    xra a    ; a = 0
    cmp m    ; compare it with memory at HL
    jnz loop ; if it's not zero, keep looping.
    mvi a, 0
    call funcao
    nop
fim:
    jmp fim
funcao:
    push psw
    adi 0xff
    mov b, a
    adi 50
    ana b
    pop psw
    ret
nop
nop
nop
nop
nop
nop
; data!!!
; Random data for tests.
zero: .db 0 ; write a byte
um: .db 1
dois: .db 2
tres: .db 3
quatro: .db 4
; write an ascii string with a null terminator
msg: .asciiz "Hello,                      World!"
