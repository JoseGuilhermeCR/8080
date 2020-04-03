#include "i8080emu.h"

/* Double Byte Add Instructions */

/* Macro for defining dad instructions */
#define DAD_INSTR(r,R,R1)	\
	INSTR(dad_##r)	{	\
		uint16_t HL = (emu->i8080.H << 8) | emu->i8080.L;	\
		uint16_t register_pair = (emu->i8080.R << 8) | emu->i8080.R1;	\
		HL += register_pair;	\
		emu->i8080.L = LB(HL);	\
		emu->i8080.H = HB(HL);	\
		i8080_set_flag(&emu->i8080, FLAG_C, (HL + register_pair) > 65535);	\
	}

DAD_INSTR(b,B,C)
DAD_INSTR(d,D,E)	

/*
Example with comments:
INSTR(dad_b) {
	// HL += register_pair;
	uint16_t HL = (emu->i8080.H << 8) | emu->i8080.L;		// Put together the values held by H and L.
	uint16_t register_pair = (emu->i8080.B << 8) | emu->i8080.C;	// Same with register_pair.

	HL += register_pair;
	
	// Put each byte in their own register.
	emu->i8080.L = LB(HL);
	emu->i8080.H = HB(HL);

	i8080_set_flag(&emu->i8080, FLAG_C, (HL + register_pair) > 65535);// Carry Flag.
}
*/
