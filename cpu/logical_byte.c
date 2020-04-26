#include "i8080emu.h"

/* Set the value of the needed flags after xor, or. */
void set_logical_flags(i8080emu *emu) {
	i8080emu_set_flag(emu, FLAG_C, false);
	i8080emu_set_flag(emu, FLAG_A, false);
	set_zsp(emu, emu->i8080.A);
}

/* Set the value of the needed flags after cmp. */
void set_logical_cmp_flags(i8080emu *emu, uint8_t value) {
	const uint16_t result = emu->i8080.A - value;
	i8080emu_set_flag(emu, FLAG_C, result >> 8);
	i8080emu_set_flag(emu, FLAG_A, ~(emu->i8080.A ^ result ^ value) & 0x10);
	set_zsp(emu, result & 0xff);
}

/* Logical And Instructions */
#define ANA_INSTR(r,R)\
	INSTR(ana_##r) {\
		uint8_t result = emu->i8080.A & emu->i8080.R;\
		i8080emu_set_flag(emu, FLAG_C, false);\
		i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.A | emu->i8080.R) & 0x08) != 0);\
		set_zsp(emu, result);\
		emu->i8080.A = result;\
		++emu->i8080.PC;\
		return 4;\
	}


ANA_INSTR(b,B)
ANA_INSTR(c,C)
ANA_INSTR(d,D)
ANA_INSTR(e,E)
ANA_INSTR(h,H)
ANA_INSTR(l,L)
ANA_INSTR(a,A)

INSTR(ana_m) {
	uint8_t byte = get_byte_hl(emu);
	uint8_t result = emu->i8080.A & byte;
	i8080emu_set_flag(emu, FLAG_C, false);
	i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.A | byte) & 0x08) != 0);
	set_zsp(emu, result);

	emu->i8080.A = result;
	++emu->i8080.PC;
	return 7;
}

INSTR(ani) {
	uint8_t byte = get_byte_from_instruction(emu);
	uint8_t result = emu->i8080.A & byte;
	i8080emu_set_flag(emu, FLAG_C, false);
	i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.A | byte) & 0x08) != 0);
	set_zsp(emu, result);

	emu->i8080.A = result;
	emu->i8080.PC += 2;
	return 7;
}

/* Logical Xor Instructions */
#define XRA_INSTR(r,R)\
	INSTR(xra_##r) {\
		emu->i8080.A ^= emu->i8080.R;\
		set_logical_flags(emu);\
		++emu->i8080.PC;\
		return 4;\
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
	set_logical_flags(emu);

	++emu->i8080.PC;

	return 7;
}

INSTR(xri) {
	emu->i8080.A ^= get_byte_from_instruction(emu);
	set_logical_flags(emu);

	emu->i8080.PC += 2;

	return 7;
}

/* Logical Or Instructions */
#define ORA_INSTR(r,R)\
	INSTR(ora_##r) {\
		emu->i8080.A |= emu->i8080.R;\
		set_logical_flags(emu);\
		++emu->i8080.PC;\
		return 4;\
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
	set_logical_flags(emu);
	
	++emu->i8080.PC;

	return 7;
}

INSTR(ori) {
	emu->i8080.A |= get_byte_from_instruction(emu);
	set_logical_flags(emu);

	emu->i8080.PC += 2;

	return 7;
}

/* Logical Compare Instructions */
#define CMP_INSTR(r,R)\
	INSTR(cmp_##r) {\
		set_logical_cmp_flags(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 4;\
	}

CMP_INSTR(b,B)
CMP_INSTR(c,C)
CMP_INSTR(d,D)
CMP_INSTR(e,E)
CMP_INSTR(h,H)
CMP_INSTR(l,L)
CMP_INSTR(a,A)

INSTR(cmp_m) {
	set_logical_cmp_flags(emu, get_byte_hl(emu));
	
	++emu->i8080.PC;

	return 7;
}

INSTR(cpi) {
	set_logical_cmp_flags(emu, get_byte_from_instruction(emu));
	
	emu->i8080.PC += 2;

	return 7;
}
