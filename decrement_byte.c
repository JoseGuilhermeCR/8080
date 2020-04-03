#include "i8080emu.h"

/* Decrement Byte Instructions */

/* Macro for defining dcr instructions */
#define DCR_INSTR(r,R)	\
	INSTR(dcr_##r) {\
		--emu->i8080.R;	\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x0);	\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);	\
		i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.R));	\
	}

DCR_INSTR(b,B)
DCR_INSTR(c,C)
DCR_INSTR(d,D)

/* Example with comments:
INSTR(dcr_b) {
	// TODO: know how the A flag should be set in a subtraction.
	// Decrement register B.
	--emu->i8080.B;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.B == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.B & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.B));			// Parity Flag.
}
*/
