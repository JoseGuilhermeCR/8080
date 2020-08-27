#define CPUNAME 8080
#define MEM_START 100h
#define ZERO 0
#define SET_STACK lxi sp,

   .org MEM_START ; This program will be loaded at memory address 0x100.

;    LOAD_WORD_I sp, 0x07FF ; Set stack, so we can make function calls.
    SET_STACK 0x7FF
    lxi h, msg ; HL = address msg.
    call print_string
fim:
    jmp fim

;#include "included.asm" ; Similar to a header file, it's content will be replaced.

; Writing a null terminated string in memory.
msg: .asciiz "Hello There, CPUNAME!"

; Writiing a word in memory.
word: .dw 0xFFEE
; Writing a byte in memory.
zero: .db ZERO
