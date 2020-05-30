; text
    .org 100h
    lxi sp, 0x07FF
    lxi h, msg
loop:
    mov a, m ; move da posição endereçada por HL para acumulador
    out 1    ; escreve acumulador em porta de saída 1
    inx h    ; incrementa hl
    xra a    ; zera a
    cmp m    ; compara com m
    jnz loop ; se não for o byte 0, string não acabou, continua o loop.
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
msg: .asciiz Hello, World!
