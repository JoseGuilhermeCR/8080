#include "i8080emu.h"

/* Set the value of the needed flags after and, xor, or. */
void set_logical_flags(i8080 *i8080) {
	i8080_set_flag(i8080, FLAG_C, false);			// Carry Flag is zeroed.
	i8080_set_flag(i8080, FLAG_Z, i8080->A == 0x00);	// Zero Flag.
	i8080_set_flag(i8080, FLAG_S, i8080->A & 0x80);		// Sign Flag.
	i8080_set_flag(i8080, FLAG_P, parity_table[i8080->A]);	// Parity Flag.
}

/* Set the value of the needed flags after cmp. */
void set_logical_cmp_flags(i8080 *i8080, uint8_t value) {
	// 8080 manual says this instruction works as if value was subtracted from the accumulator,
	// and the flags set according to the result.
	uint8_t result = i8080->A - value;

	i8080_set_flag(i8080, FLAG_Z, result == 0x00);		// Zero Flag.
	i8080_set_flag(i8080, FLAG_S, result & 0x80);		// Sign Flag.
	i8080_set_flag(i8080, FLAG_P, parity_table[result]);	// Parity Flag.

	bool carry = value > i8080->A;
	bool equal_sign = (i8080->A & 0x80) && (value & 0x80);

	// Carry has it's value reversed if the numbers differ in sign.
	i8080_set_flag(i8080, FLAG_C, (equal_sign) ? carry : !carry);	// Carry Flag.
	// TODO: I think this could be wrong, I should be aware so I can check later!
	i8080_set_flag(i8080, FLAG_A, LB(value) > LB(i8080->A));	// Auxiliary Carry Flag.
}

/* Logical And Instructions */
#define ANA_INSTR(r,R)	\
	INSTR(ana_##r) {			\
		emu->i8080.A &= emu->i8080.R;	\
		set_logical_flags(&emu->i8080);	\
		++emu->i8080.PC;		\
		return 4;			\
	}


ANA_INSTR(b,B)
ANA_INSTR(c,C)
ANA_INSTR(d,D)
ANA_INSTR(e,E)
ANA_INSTR(h,H)
ANA_INSTR(l,L)
ANA_INSTR(a,A)

INSTR(ana_m) {
	emu->i8080.A &= get_byte_hl(emu);
	set_logical_flags(&emu->i8080);

	++emu->i8080.PC;

	return 7;
}

INSTR(ani) {
	emu->i8080.A &= get_byte_from_instruction(emu);
	set_logical_flags(&emu->i8080);

	emu->i8080.PC += 2;

	return 7;
}

/* Logical Xor Instructions */
#define XRA_INSTR(r,R)	\
	INSTR(xra_##r) {			\
		emu->i8080.A ^= emu->i8080.R;	\
		set_logical_flags(&emu->i8080);	\
		++emu->i8080.PC;		\
		return 4;			\
	}

XRA_INSTR(b,B)
XRA_INSTR(c,C)
XRA_INSTR(d,D)
XRA_INSTR(e,E)
XRA_INSTR(h,H)
XRA_INSTR(l,L)
XRA_INSTR(a,A)

INSTR(xra_m) {
	emu->i8080.A ^= get_byte_hl(emu);
	set_logical_flags(&emu->i8080);

	++emu->i8080.PC;

	return 7;
}

INSTR(xri) {
	emu->i8080.A ^= get_byte_from_instruction(emu);
	set_logical_flags(&emu->i8080);

	emu->i8080.PC += 2;

	return 7;
}

/* Logical Or Instructions */
#define ORA_INSTR(r,R)	\
	INSTR(ora_##r) {			\
		emu->i8080.A |= emu->i8080.R;	\
		set_logical_flags(&emu->i8080);	\
		++emu->i8080.PC;		\
		return 4;			\
	}

ORA_INSTR(b,B)
ORA_INSTR(c,C)
ORA_INSTR(d,D)
ORA_INSTR(e,E)
ORA_INSTR(h,H)
ORA_INSTR(l,L)
ORA_INSTR(a,A)

INSTR(ora_m) {
	emu->i8080.A |= get_byte_hl(emu);
	set_logical_flags(&emu->i8080);
	
	++emu->i8080.PC;

	return 7;
}

INSTR(ori) {
	emu->i8080.A |= get_byte_from_instruction(emu);
	set_logical_flags(&emu->i8080);

	emu->i8080.PC += 2;

	return 7;
}

/* Logical Compare Instructions */
#define CMP_INSTR(r,R)	\
	INSTR(cmp_##r) {					 	\
		set_logical_cmp_flags(&emu->i8080, emu->i8080.R);	\
		++emu->i8080.PC;					\
		return 4;						\
	}

CMP_INSTR(b,B)
CMP_INSTR(c,C)
CMP_INSTR(d,D)
CMP_INSTR(e,E)
CMP_INSTR(h,H)
CMP_INSTR(l,L)
CMP_INSTR(a,A)

INSTR(cmp_m) {
	set_logical_cmp_flags(&emu->i8080, get_byte_hl(emu));
	
	++emu->i8080.PC;

	return 7;
}

INSTR(cpi) {
	set_logical_cmp_flags(&emu->i8080, get_byte_from_instruction(emu));
	
	emu->i8080.PC += 2;

	return 7;
}
