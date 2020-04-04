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
MVI_INSTR(e,E)
MVI_INSTR(h,H)
MVI_INSTR(l,L)
MVI_INSTR(a,A)

/* Examples with comments:
INSTR(mvi_b) {
	// Places byte in register B.
	emu->i8080.B = emu->memory[++emu->i8080.PC];
}*/

INSTR(mvi_m) {
	// Stores byte in (HL).
	emu->memory[(emu->i8080.H << 8) | emu->i8080.L] = emu->memory[++emu->i8080.PC];
}

/* Macro for defining the stax instructions */
#define STAX_INSTR(r,R,R1)	\
	INSTR(stax_##r) {	\
		emu->memory[(emu->i8080.R << 8) | emu->i8080.R1] = emu->i8080.A;	\
	}

STAX_INSTR(b,B,C)
STAX_INSTR(d,D,E)

/* Examples with comments:
INSTR(stax_d) {
	// Moves the value stored in A to memory at (DE).
	emu->memory[(emu->i8080.D << 8) | emu->i8080.E] = emu->i8080.A;
}*/

INSTR(sta) {
	// Stores A in addr.
	uint16_t addr = emu->memory[++emu->i8080.PC];
	addr |= emu->memory[++emu->i8080.PC] << 8;

	emu->memory[addr] = emu->i8080.A;
}

/* Macro for defining the ldax instructions */
#define LDAX_INSTR(r,R,R1)	\
	INSTR(ldax_##r)	{	\
		emu->i8080.A = emu->memory[(emu->i8080.R << 8) | emu->i8080.R1];	\
	}

LDAX_INSTR(b,B,C)
LDAX_INSTR(d,D,E)

/* Examples with comments:
INSTR(ldax_d) {
	// Moves the value from memory at (DE) to A.
	emu->i8080.A = emu->memory[(emu->i8080.D << 8) | emu->i8080.E];
}*/

INSTR(lda) {
	// Loads byte in (word) to A.
	uint16_t addr = emu->memory[++emu->i8080.PC];
	addr |= emu->memory[++emu->i8080.PC] << 8;

	emu->i8080.A = emu->memory[addr];
}