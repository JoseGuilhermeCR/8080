    .org 100h ; This program will be loaded at memory address 0x100.

    lxi sp, 0x07FF ; Set stack, so we can make function calls.
    lxi h, msg ; HL = address msg.
    call print_string
fim:
    jmp fim
print_string:
    push psw    
loop:
    mov a, m ; Move from memory at HL to accumulator.
    out 0    ; Write accumulator to io port 0.
    inx h    ; Increment HL.
    xra a    ; a = 0.
    cmp m    ; Compare a with memory at HL.
    jnz loop ; If it's not zero, continue looping.
    pop psw
    ret
; Writing a null terminated string in memory.
msg: .asciiz "Hello There, 8080!"
; Writiing a word in memory.
word: .dw 0xFFEE
; Writing a byte in memory.
byte: .db 0xAA
