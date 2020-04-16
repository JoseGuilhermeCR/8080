#include "i8080emu.h"

/* Increment byte instructions */

/* Macro for defining the inr instruction. */
#define INR_INSTR(r,R)										\
	INSTR(inr_##r) {									\
		i8080_set_flag(&emu->i8080, FLAG_A, LN(emu->i8080.R) + 1 > 15);			\
		++emu->i8080.R;									\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x00);			\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);			\
                i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.R]);		\
		++emu->i8080.PC;								\
		return 5;									\
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
	uint8_t byte = get_byte_hl(emu);						// Get byte from memory

	i8080_set_flag(&emu->i8080, FLAG_A, LN(byte) + 1 > 15);	

	++byte;
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, byte);	// Save incremented byte.

	i8080_set_flag(&emu->i8080, FLAG_Z, byte == 0x00);		// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, byte & 0x80);		// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[byte]);	// Parity Flag.

	++emu->i8080.PC;

	return 10;
}

/* Decrement Byte Instructions */

/* Macro for defining dcr instructions */
#define DCR_INSTR(r,R)									\
	INSTR(dcr_##r) {								\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.R) + 0xf) & 0x10);	\
		--emu->i8080.R;								\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x0);		\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);		\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.R]);	\
		++emu->i8080.PC;							\
		return 5;								\
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
	uint8_t byte = get_byte_hl(emu);						// Get byte from memory

	// Adds LN of byte with LN of -1 in Two's complement (0xff -> LN = 0xf) and checks if there was a carry out to bit four.
	// It's easier to work this out with addition!!
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(byte) + 0xf) & 0x10);

	--byte;
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, byte);	// Save incremented byte.

	i8080_set_flag(&emu->i8080, FLAG_Z, byte == 0x00);		// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, byte & 0x80);		// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[byte]);	// Parity Flag.
	++emu->i8080.PC;

	return 10;
}

/* Add Byte Instructions */
#define ADD_INSTR(r,R)												\
	INSTR(add_##r) {											\
		i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + emu->i8080.R) > 255);			\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(emu->i8080.R)) & 0x10);		\
		emu->i8080.A += emu->i8080.R;									\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);					\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);					\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);				\
		++emu->i8080.PC;										\
		return 4;											\
	}

ADD_INSTR(b,B)
ADD_INSTR(c,C)
ADD_INSTR(d,D)
ADD_INSTR(e,E)
ADD_INSTR(h,H)
ADD_INSTR(l,L)
ADD_INSTR(a,A)

INSTR(add_m) {
	uint8_t byte = get_byte_hl(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + byte) > 255);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte)) & 0x10);		// Auxiliary Carry Flag.

	emu->i8080.A += byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	++emu->i8080.PC;

	return 7;
}

INSTR(adi) {
	// Adds an immediat to A.
	uint8_t byte = get_byte_from_instruction(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + byte) > 255);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte)) & 0x10);		// Auxiliary Carry Flag.

	emu->i8080.A += byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	emu->i8080.PC += 2;

	return 7;
}

/* Add Byte with Carry-In Instructions */
#define ADC_INSTR(r,R)													\
	INSTR(adc_##r) {												\
		uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;						\
		i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + emu->i8080.R + carry) > 255);			\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(emu->i8080.R) + carry) & 0x10);		\
		emu->i8080.A += emu->i8080.R + carry;									\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);						\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);						\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);					\
		++emu->i8080.PC;											\
		return 4;												\
	}

ADC_INSTR(b,B)
ADC_INSTR(c,C)
ADC_INSTR(d,D)
ADC_INSTR(e,E)
ADC_INSTR(h,H)
ADC_INSTR(l,L)
ADC_INSTR(a,A)

INSTR(adc_m) {
	uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;
	uint8_t byte = get_byte_hl(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + byte + carry) > 255);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte) + carry) & 0x10);		// Auxiliary Carry Flag.

	emu->i8080.A += byte + carry;
	
	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);					// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);					// Signal Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);				// Parity Flag.

	++emu->i8080.PC;

	return 7;
}

INSTR(aci) {
	// Adds an immediat to A and carry.
	uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;
	uint8_t byte = get_byte_from_instruction(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + byte + carry) > 255);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte) + carry) & 0x10);		// Auxiliary Carry Flag.

	emu->i8080.A += byte + carry;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	emu->i8080.PC += 2;

	return 7;
}

/* Sub Byte Instructions */
#define SUB_INSTR(r,R)										\
	INSTR(sub_##r) {									\
		i8080_set_flag(&emu->i8080, FLAG_C,  emu->i8080.A < emu->i8080.R);		\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(C2(emu->i8080.R))) & 0X10);\
		emu->i8080.A -= emu->i8080.R;							\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);			\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);			\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);		\
		++emu->i8080.PC;								\
		return 4;									\
	}

SUB_INSTR(b,B)
SUB_INSTR(c,C)
SUB_INSTR(d,D)
SUB_INSTR(e,E)
SUB_INSTR(h,H)
SUB_INSTR(l,L)
SUB_INSTR(a,A)

INSTR(sub_m) {
	uint8_t byte = get_byte_hl(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, emu->i8080.A < byte);				// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(C2(byte))) & 0X10);		// Auxiliary Carry Flag.

	emu->i8080.A -= byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	++emu->i8080.PC;

	return 7;
}

INSTR(sui) {
	uint8_t byte = get_byte_from_instruction(emu);
	
	i8080_set_flag(&emu->i8080, FLAG_C, emu->i8080.A < byte);				// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(C2(byte))) & 0X10);		// Auxiliary Carry Flag.

	emu->i8080.A -= byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	emu->i8080.PC += 2;

	return 7;
}

/* Sub Byte with Borrow In Instructions */
#define SBB_INSTR(r,R)											\
	INSTR(sbb_##r) {										\
		uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;				\
		i8080_set_flag(&emu->i8080, FLAG_C, emu->i8080.A < emu->i8080.R - carry);		\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(C2(emu->i8080.R + carry)) + LN(emu->i8080.A)) & 0x10);\
		emu->i8080.A = emu->i8080.A - emu->i8080.R - carry;					\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			\
		++emu->i8080.PC;									\
		return 4;										\
	}

SBB_INSTR(b,B)
SBB_INSTR(c,C)
SBB_INSTR(d,D)
SBB_INSTR(e,E)
SBB_INSTR(h,H)
SBB_INSTR(l,L)
SBB_INSTR(a,A)

INSTR(sbb_m) {
	uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;
	uint8_t byte = get_byte_hl(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, emu->i8080.A < byte - carry);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(C2(byte + carry)) + LN(emu->i8080.A)) & 0x10);	// Auxiliary Carry Flag.

	emu->i8080.A = (emu->i8080.A - byte) - carry;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	++emu->i8080.PC;

	return 7;
}

INSTR(sbi) {
	uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;
	uint8_t byte = get_byte_from_instruction(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, emu->i8080.A < byte - carry);
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(C2(byte + carry)) + LN(emu->i8080.A)) & 0x10);

	emu->i8080.A = (emu->i8080.A - byte) - carry;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);

	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);
	emu->i8080.PC += 2;

	return 7;
}

INSTR(daa) {
	uint8_t value = emu->i8080.A;

	if (LN(value) > 9 || i8080_get_flag(&emu->i8080, FLAG_A)) {
		if (LN(value) + 6 > 15)
			i8080_set_flag(&emu->i8080, FLAG_A, true);
		else
			i8080_set_flag(&emu->i8080, FLAG_A, false);

		value += 6;
	}

	if (HN(value) > 9 || i8080_get_flag(&emu->i8080, FLAG_C)) {
		if (value + 96 > 255)
			i8080_set_flag(&emu->i8080, FLAG_C, true);
		else
			i8080_set_flag(&emu->i8080, FLAG_C, false);

		value += 96;
	}

	emu->i8080.A = value;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);

	++emu->i8080.PC;

	return 4;
}
