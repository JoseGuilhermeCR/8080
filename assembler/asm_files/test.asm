   .org 100h ; This program will be loaded at memory address 0x100.

    lxi sp, 0x07FF ; Set stack, so we can make function calls.
    lxi h, msg ; HL = address msg.
    call print_string
fim:
    jmp fim
#include "included.asm" ; Similar to a header file, it's content will be replaced.

; Writing a null terminated string in memory.
msg: .asciiz "Hello There, 8080!"
; Writiing a word in memory.
word: .dw 0xFFEE
; Writing a byte in memory.
byte: .db 0xAA
