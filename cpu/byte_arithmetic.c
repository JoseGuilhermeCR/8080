#include "i8080emu.h"

/* Increment byte instructions */

/* Macro for defining the inr instruction. */
#define INR_INSTR(r,R)\
	INSTR(inr_##r) {\
		i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.R ^ 0x1) & 0x10) != ((emu->i8080.R + 0x1) & 0x10));\
		++emu->i8080.R;\
		set_zsp(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 5;\
	}

/* Increment Byte Instructions */
INR_INSTR(b,B)
INR_INSTR(c,C)
INR_INSTR(d,D)
INR_INSTR(e,E)
INR_INSTR(h,H)
INR_INSTR(l,L)
INR_INSTR(a,A)

INSTR(inr_m) {
	// Increments byte at (HL).
	uint8_t byte = get_byte_hl(emu);	// Get byte from memory
	i8080emu_set_flag(emu, FLAG_A, ((byte ^ 0x1) & 0x10) != ((byte + 0x1) & 0x10));

	++byte;
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, byte);	// Save incremented byte.

	set_zsp(emu, byte);
	++emu->i8080.PC;

	return 10;
}

/* Decrement Byte Instructions */

/* Macro for defining dcr instructions */
#define DCR_INSTR(r,R)\
	INSTR(dcr_##r) {\
		i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.R ^ 0xff) & 0x10) != ((emu->i8080.R + 0xff) & 0x10));\
		--emu->i8080.R;\
		set_zsp(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 5;\
	}

DCR_INSTR(b,B)
DCR_INSTR(c,C)
DCR_INSTR(d,D)
DCR_INSTR(e,E)
DCR_INSTR(h,H)
DCR_INSTR(l,L)
DCR_INSTR(a,A)

INSTR(dcr_m) {
	// Decrements byte at (HL).
	uint8_t byte = get_byte_hl(emu);	// Get byte from memory
	i8080emu_set_flag(emu, FLAG_A, ((byte ^ 0xff) & 0x10) != ((byte + 0xff) & 0x10));

	--byte;
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, byte);	// Save incremented byte.

	set_zsp(emu, byte);
	++emu->i8080.PC;

	return 10;
}

bool carry(int bit_no, uint8_t a, uint8_t b, bool cy) {
	uint16_t result = a + b + cy;
	uint16_t carry = result ^ a ^ b;
	return carry & (1 << bit_no);
}

void add(i8080emu *emu, uint8_t *reg, uint8_t a, uint8_t cy) {
	const uint8_t result = *reg + a + cy;

	i8080emu_set_flag(emu, FLAG_C, carry(8, *reg, a, cy));
	i8080emu_set_flag(emu, FLAG_A, carry(4, *reg, a, cy));

	set_zsp(emu, result);

	*reg = result;
}

void sub(i8080emu *emu, uint8_t *reg, uint8_t a, uint8_t cy) {
	add(emu, reg, ~a, (cy == 0) ? 1 : 0);
	i8080emu_set_flag(emu, FLAG_C, !i8080emu_get_flag(emu, FLAG_C));
}

/* Add Byte Instructions */
#define ADD_INSTR(r,R)\
	INSTR(add_##r) {\
		add(emu, &emu->i8080.A, emu->i8080.R, 0);\
		++emu->i8080.PC;\
		return 4;\
	}

ADD_INSTR(b,B)
ADD_INSTR(c,C)
ADD_INSTR(d,D)
ADD_INSTR(e,E)
ADD_INSTR(h,H)
ADD_INSTR(l,L)
ADD_INSTR(a,A)

INSTR(add_m) {
	add(emu, &emu->i8080.A, get_byte_hl(emu), 0);
	++emu->i8080.PC;
	return 7;
}

INSTR(adi) {
	// Adds an immediat to A.
	add(emu, &emu->i8080.A, get_byte_from_instruction(emu), 0);
	emu->i8080.PC += 2;
	return 7;
}

/* Add Byte with Carry-In Instructions */
#define ADC_INSTR(r,R)\
	INSTR(adc_##r) {\
		add(emu, &emu->i8080.A, emu->i8080.R, i8080emu_get_flag(emu, FLAG_C));\
		++emu->i8080.PC;\
		return 4;\
	}

ADC_INSTR(b,B)
ADC_INSTR(c,C)
ADC_INSTR(d,D)
ADC_INSTR(e,E)
ADC_INSTR(h,H)
ADC_INSTR(l,L)
ADC_INSTR(a,A)

INSTR(adc_m) {
	add(emu, &emu->i8080.A, get_byte_hl(emu), i8080emu_get_flag(emu, FLAG_C));
	++emu->i8080.PC;
	return 7;
}

INSTR(aci) {
	// Adds an immediat to A and carry.
	add(emu, &emu->i8080.A, get_byte_from_instruction(emu), i8080emu_get_flag(emu, FLAG_C));
	emu->i8080.PC += 2;
	return 7;
}

/* Sub Byte Instructions */
#define SUB_INSTR(r,R)\
	INSTR(sub_##r) {\
		sub(emu, &emu->i8080.A, emu->i8080.R, 0);\
		++emu->i8080.PC;\
		return 4;\
	}

SUB_INSTR(b,B)
SUB_INSTR(c,C)
SUB_INSTR(d,D)
SUB_INSTR(e,E)
SUB_INSTR(h,H)
SUB_INSTR(l,L)
SUB_INSTR(a,A)

INSTR(sub_m) {
	sub(emu, &emu->i8080.A, get_byte_hl(emu), 0);
	++emu->i8080.PC;
	return 7;
}

INSTR(sui) {
	sub(emu, &emu->i8080.A, get_byte_from_instruction(emu), 0);
	emu->i8080.PC += 2;
	return 7;
}

/* Sub Byte with Borrow In Instructions */
#define SBB_INSTR(r,R)\
	INSTR(sbb_##r) {\
		sub(emu, &emu->i8080.A, emu->i8080.R, i8080emu_get_flag(emu, FLAG_C));\
		++emu->i8080.PC;\
		return 4;\
	}

SBB_INSTR(b,B)
SBB_INSTR(c,C)
SBB_INSTR(d,D)
SBB_INSTR(e,E)
SBB_INSTR(h,H)
SBB_INSTR(l,L)
SBB_INSTR(a,A)

INSTR(sbb_m) {
	sub(emu, &emu->i8080.A, get_byte_hl(emu), i8080emu_get_flag(emu, FLAG_C));
	++emu->i8080.PC;
	return 7;
}

INSTR(sbi) {
	sub(emu, &emu->i8080.A, get_byte_from_instruction(emu), i8080emu_get_flag(emu, FLAG_C));
	emu->i8080.PC += 2;
	return 7;
}

INSTR(daa) {
	bool cy = i8080emu_get_flag(emu, FLAG_C);
	uint8_t correction = 0;

	if (LN(emu->i8080.A) > 9 || i8080emu_get_flag(emu, FLAG_A)) {
		correction += 6;
	}

	if (HN(emu->i8080.A) > 9 || (HN(emu->i8080.A) >= 9 && LN(emu->i8080.A) > 9) || cy) {
		correction += 96;
		cy = true;
	}

	add(emu, &emu->i8080.A, correction, 0);
	i8080emu_set_flag(emu, FLAG_C, cy);

	++emu->i8080.PC;

	return 4;
}
