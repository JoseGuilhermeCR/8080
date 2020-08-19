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
