#include "i8080emu.h"

/* Increment Word Instructions */

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

INSTR(inx_sp) {
	++emu->i8080.SP;
}

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
DAD_INSTR(h,H,L)

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

INSTR(dad_sp) {
	// HL += SP;
	uint16_t HL = (emu->i8080.H << 8) | emu->i8080.L;	// Put together the values held by H and L.

	HL += emu->i8080.SP;
	
	// Put each byte in their own register.
	emu->i8080.L = LB(HL);
	emu->i8080.H = HB(HL);

	i8080_set_flag(&emu->i8080, FLAG_C, (HL + emu->i8080.SP) > 65535);// Carry Flag.
}
