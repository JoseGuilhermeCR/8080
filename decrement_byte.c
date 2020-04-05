#include "i8080emu.h"

/* Decrement Byte Instructions */

/* Macro for defining dcr instructions */
#define DCR_INSTR(r,R)	\
	INSTR(dcr_##r) {\
		--emu->i8080.R;	\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x0);	\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);	\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.R]);	\
	}

DCR_INSTR(b,B)
DCR_INSTR(c,C)
DCR_INSTR(d,D)
DCR_INSTR(e,E)
DCR_INSTR(h,H)
DCR_INSTR(l,L)
DCR_INSTR(a,A)

/* Example with comments:
INSTR(dcr_b) {
	// TODO: know how the A flag should be set in a subtraction.
	// Decrement register B.
	--emu->i8080.B;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.B == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.B & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.B]);		// Parity Flag.
}
*/

INSTR(dcr_m) {
	// Decrements byte at (HL).
	// Points to the wanted byte.
	uint8_t *byte = &emu->memory[(emu->i8080.H << 8) | emu->i8080.L];

	// TODO: know how the A flag should be set in a subtraction.

	--(*byte);

	i8080_set_flag(&emu->i8080, FLAG_Z, *byte == 0x00);		// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, *byte & 0x80);		// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[*byte]);	// Parity Flag.
}
