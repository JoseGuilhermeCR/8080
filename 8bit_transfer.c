#include "i8080emu.h"

/* 8bit transfer instructions */

/* Macro for defining the mvi instructions */
#define MVI_INSTR(r,R)	\
	INSTR(mvi_##r) {\
		emu->i8080.R = emu->memory[++emu->i8080.PC];	\
	}

MVI_INSTR(b,B)
MVI_INSTR(c,C)
MVI_INSTR(d,D)

/* Macro for defining the stax instructions */
#define STAX_INSTR(r,R,R1)	\
	INSTR(stax_##r) {	\
		emu->memory[(emu->i8080.R << 8) | emu->i8080.R1] = emu->i8080.A;	\
	}

STAX_INSTR(b,B,C)
STAX_INSTR(d,D,E)

/* Macro for defining the ldax instructions */
#define LDAX_INSTR(r,R,R1)	\
	INSTR(ldax_##r)	{	\
		emu->i8080.A = emu->memory[(emu->i8080.R << 8) | emu->i8080.R1];	\
	}

LDAX_INSTR(b,B,C)
LDAX_INSTR(d,D,E)

/* Examples with comments:
INSTR(mvi_b) {
	// Places byte in register B.
	emu->i8080.B = emu->memory[++emu->i8080.PC];
}

INSTR(stax_d) {
	// Moves the value stored in A to memory at (DE).
	emu->memory[(emu->i8080.D << 8) | emu->i8080.E] = emu->i8080.A;
}

INSTR(ldax_d) {
	// Moves the value from memory at (DE) to A.
	emu->i8080.A = emu->memory[(emu->i8080.D << 8) | emu->i8080.E];
}
*/
