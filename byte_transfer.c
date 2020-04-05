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

#define MOV_INSTR(r,r1,R,R1)	\
	INSTR(mov_##r##r1) {	\
		emu->i8080.R = emu->i8080.R1;	\
	}

#define MOV_RM_INSTR(r,r1,R)	\
	INSTR(mov_##r##r1) {	\
		emu->i8080.R = emu->memory[(emu->i8080.H << 8) | emu->i8080.L];	\
	}

#define MOV_MR_INSTR(r,r1,R)	\
	INSTR(mov_##r##r1) {	\
		emu->memory[(emu->i8080.H << 8) | emu->i8080.L] = emu->i8080.R;	\
	}

MOV_INSTR(b,c,B,C)
MOV_INSTR(b,d,B,D)
MOV_INSTR(b,e,B,E)
MOV_INSTR(b,h,B,H)
MOV_INSTR(b,l,B,L)
MOV_RM_INSTR(b,m,B)
MOV_INSTR(b,a,B,A)

MOV_INSTR(c,b,C,B)
MOV_INSTR(c,d,C,D)
MOV_INSTR(c,e,C,E)
MOV_INSTR(c,h,C,H)
MOV_INSTR(c,l,C,L)
MOV_RM_INSTR(c,m,C)
MOV_INSTR(c,a,C,A)

MOV_INSTR(d,b,D,B)
MOV_INSTR(d,c,D,C)
MOV_INSTR(d,e,D,E)
MOV_INSTR(d,h,D,H)
MOV_INSTR(d,l,D,L)
MOV_RM_INSTR(d,m,D)
MOV_INSTR(d,a,D,A)

MOV_INSTR(e,b,E,B)
MOV_INSTR(e,c,E,C)
MOV_INSTR(e,d,E,D)
MOV_INSTR(e,h,E,H)
MOV_INSTR(e,l,E,L)
MOV_RM_INSTR(e,m,E)
MOV_INSTR(e,a,E,A)

MOV_INSTR(h,b,H,B)
MOV_INSTR(h,c,H,C)
MOV_INSTR(h,d,H,D)
MOV_INSTR(h,e,H,E)
MOV_INSTR(h,l,H,L)
MOV_RM_INSTR(h,m,H)
MOV_INSTR(h,a,H,A)

MOV_INSTR(l,b,L,B)
MOV_INSTR(l,c,L,C)
MOV_INSTR(l,d,L,D)
MOV_INSTR(l,e,L,E)
MOV_INSTR(l,h,L,H)
MOV_RM_INSTR(l,m,L)
MOV_INSTR(l,a,L,A)

MOV_MR_INSTR(m,b,B)
MOV_MR_INSTR(m,c,C)
MOV_MR_INSTR(m,d,D)
MOV_MR_INSTR(m,e,E)
MOV_MR_INSTR(m,h,H)
MOV_MR_INSTR(m,l,L)
MOV_MR_INSTR(m,a,A)