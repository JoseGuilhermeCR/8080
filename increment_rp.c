#include "i8080emu.h"

/* Increment Register Pair Instructions */

/* Macro for defining inx instructions */
#define INX_INSTR(r,R,R1)	\
	INSTR(inx_##r) {	\
		uint16_t register_pair = ((emu->i8080.R << 8) | emu->i8080.R1) + 1;	\
		emu->i8080.R1 = LB(register_pair);	\
		emu->i8080.R  = HB(register_pair);	\
	}

INX_INSTR(b,B,C)
INX_INSTR(d,D,E)
INX_INSTR(h,H,L)

/*
Example with comments:
INSTR(inx_d) {
	// Increment register pair DE
	uint16_t register_pair = ((emu->i8080.D << 8) | emu->i8080.E) + 1;
	emu->i8080.D = LB(register_pair);
	emu->i8080.E = HB(register_pair);
}
*/
