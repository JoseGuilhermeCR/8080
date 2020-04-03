#include "i8080emu.h"

/* Increment byte instructions */

/* Macro for defining the inr instruction. */
#define INR_INSTR(r,R)		\
	INSTR(inr_##r) {	\
		i8080_set_flag(&emu->i8080, FLAG_A, carry_out_lower_nibble(emu->i8080.R, 1));	\
		++emu->i8080.R;									\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x00);			\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);			\
                i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.R));			\
	}

/* Increment Byte Instructions */
INR_INSTR(b,B)
INR_INSTR(c,C)
INR_INSTR(d,D)
INR_INSTR(e,E)
INR_INSTR(h,H)

/* For example: INR_INSTR(b,B) will become (here in a more readable way and with comments):
INSTR(inr_b) {
	i8080_set_flag(&emu->i8080, FLAG_A, carry_out_lower_nibble(emu->i8080.B, 1));	// Auxiliary Carry Flag.

	// Increment register B.
	++emu->i8080.B;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.B == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.B & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.B));			// Parity Flag.
} */
