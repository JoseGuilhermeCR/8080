#include "i8080emu.h"

/* 8bit transfer instructions */

/* Macro for defining the mvi instructions */
#define MVI_INSTR(r,R)	\
	INSTR(mvi_##r) {\
		emu->i8080.R = get_byte_from_instruction(emu);	\
		emu->i8080.PC += 2;				\
		return 7;					\
	}

MVI_INSTR(b,B)
MVI_INSTR(c,C)
MVI_INSTR(d,D)
MVI_INSTR(e,E)
MVI_INSTR(h,H)
MVI_INSTR(l,L)
MVI_INSTR(a,A)

INSTR(mvi_m) {
	// Stores byte in (HL).
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, get_byte_from_instruction(emu));
	emu->i8080.PC += 2;

	return 10;
}

/* Macro for defining the stax instructions */
#define STAX_INSTR(r,R,R1)	\
	INSTR(stax_##r) {	\
		i8080emu_write_byte_memory(emu, (emu->i8080.R << 8) | emu->i8080.R1, emu->i8080.A);	\
		++emu->i8080.PC;									\
		return 7;										\
	}

STAX_INSTR(b,B,C)
STAX_INSTR(d,D,E)

INSTR(sta) {
	// Stores A in addr.
	i8080emu_write_byte_memory(emu, get_word_from_instruction(emu), emu->i8080.A);
	emu->i8080.PC += 3;

	return 13;
}

/* Macro for defining the ldax instructions */
#define LDAX_INSTR(r,R,R1)	\
	INSTR(ldax_##r)	{	\
		emu->i8080.A = i8080emu_read_byte_memory(emu, (emu->i8080.R << 8) | emu->i8080.R1);	\
		++emu->i8080.PC;									\
		return 7;										\
	}

LDAX_INSTR(b,B,C)
LDAX_INSTR(d,D,E)

INSTR(lda) {
	// Loads byte in (word) to A.
	emu->i8080.A = i8080emu_read_byte_memory(emu, get_word_from_instruction(emu));
	emu->i8080.PC += 3;

	return 13;
}

#define MOV_INSTR(r,r1,R,R1)	\
	INSTR(mov_##r##r1) {	\
		emu->i8080.R = emu->i8080.R1;	\
		++emu->i8080.PC;		\
		return 5;			\
	}

#define MOV_RM_INSTR(r,r1,R)	\
	INSTR(mov_##r##r1) {	\
		emu->i8080.R = get_byte_hl(emu);	\
		++emu->i8080.PC;			\
		return 7;				\
	}

#define MOV_MR_INSTR(r,r1,R)	\
	INSTR(mov_##r##r1) {	\
		i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, emu->i8080.R);	\
		++emu->i8080.PC;									\
		return 7;										\
	}

MOV_INSTR(b,b,B,B)
MOV_INSTR(b,c,B,C)
MOV_INSTR(b,d,B,D)
MOV_INSTR(b,e,B,E)
MOV_INSTR(b,h,B,H)
MOV_INSTR(b,l,B,L)
MOV_RM_INSTR(b,m,B)
MOV_INSTR(b,a,B,A)

MOV_INSTR(c,b,C,B)
MOV_INSTR(c,c,C,C)
MOV_INSTR(c,d,C,D)
MOV_INSTR(c,e,C,E)
MOV_INSTR(c,h,C,H)
MOV_INSTR(c,l,C,L)
MOV_RM_INSTR(c,m,C)
MOV_INSTR(c,a,C,A)

MOV_INSTR(d,b,D,B)
MOV_INSTR(d,c,D,C)
MOV_INSTR(d,d,D,D)
MOV_INSTR(d,e,D,E)
MOV_INSTR(d,h,D,H)
MOV_INSTR(d,l,D,L)
MOV_RM_INSTR(d,m,D)
MOV_INSTR(d,a,D,A)

MOV_INSTR(e,b,E,B)
MOV_INSTR(e,c,E,C)
MOV_INSTR(e,d,E,D)
MOV_INSTR(e,e,E,E)
MOV_INSTR(e,h,E,H)
MOV_INSTR(e,l,E,L)
MOV_RM_INSTR(e,m,E)
MOV_INSTR(e,a,E,A)

MOV_INSTR(h,b,H,B)
MOV_INSTR(h,c,H,C)
MOV_INSTR(h,d,H,D)
MOV_INSTR(h,e,H,E)
MOV_INSTR(h,h,H,H)
MOV_INSTR(h,l,H,L)
MOV_RM_INSTR(h,m,H)
MOV_INSTR(h,a,H,A)

MOV_INSTR(l,b,L,B)
MOV_INSTR(l,c,L,C)
MOV_INSTR(l,d,L,D)
MOV_INSTR(l,e,L,E)
MOV_INSTR(l,h,L,H)
MOV_INSTR(l,l,L,L)
MOV_RM_INSTR(l,m,L)
MOV_INSTR(l,a,L,A)

MOV_MR_INSTR(m,b,B)
MOV_MR_INSTR(m,c,C)
MOV_MR_INSTR(m,d,D)
MOV_MR_INSTR(m,e,E)
MOV_MR_INSTR(m,h,H)
MOV_MR_INSTR(m,l,L)
MOV_MR_INSTR(m,a,A)

MOV_INSTR(a,b,A,B)
MOV_INSTR(a,c,A,C)
MOV_INSTR(a,d,A,D)
MOV_INSTR(a,e,A,E)
MOV_INSTR(a,h,A,H)
MOV_INSTR(a,l,A,L)
MOV_RM_INSTR(a,m,A)
MOV_INSTR(a,a,A,A)
