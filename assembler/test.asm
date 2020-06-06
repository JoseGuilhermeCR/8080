; text
    .ORG 100H
    lxi sp, 0x07FF
    lxi h, msg
loop:
    mov a, m ; test a aa a aaa
    out 1    
    INX H
    xra a    
    cmp m    
    jnz loop 
    mvi a, 0
    call funcao
    nop
fim:
    jmp fim
    push psw
    adi 0xff
    mov b, a
    adi 50
    ana b
    pop psw
    ret
nop
funcao:
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
