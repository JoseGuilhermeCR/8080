; This should be called by RST 0.
push	psw
mvi	d, 0xFF
mov	a, d
cmp	d
ei
pop	psw
ret

.org 0x100
lxi sp, 0x0FF7
inr a
dcr a
hlt
nop
