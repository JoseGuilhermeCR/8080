#include "i8080emu.h"

/* Decrement Word Instructions */

/* Macro for defining dcx instructions */
#define DCX_INSTR(r,R,R1)	\
	INSTR(dcx_##r) {	\
		uint16_t register_pair = ((emu->i8080.R << 8) | emu->i8080.R1) - 1;	\
		emu->i8080.R1 = LB(register_pair);	\
		emu->i8080.R  = HB(register_pair);	\
	}

DCX_INSTR(b,B,C)
DCX_INSTR(d,D,E)
DCX_INSTR(h,H,L)

/*
Example with comments:
INSTR(dcx_b) {
	// Decrement register pair BC.
	uint16_t register_pair = ((emu->i8080.B << 8) | emu->i8080.C) - 1;
	emu->i8080.C = LB(register_pair);
	emu->i8080.B = HB(register_pair);
}
*/

INSTR(dcx_sp) {
	--emu->i8080.SP;
}