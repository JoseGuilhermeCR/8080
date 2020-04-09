#include "i8080emu.h"

/* TODO: Review flags in Subtraction (maybe change to two's complement to make it easier to follow the 8080 manual.).
	 Review ADC (carry may occur when we add the current carry). 
	 Try to simplify the instruction methods by using another function to set the flags. */

/* Increment byte instructions */

/* Macro for defining the inr instruction. */
#define INR_INSTR(r,R)		\
	INSTR(inr_##r) {	\
		++emu->i8080.R;									\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x00);			\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);			\
                i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.R]);		\
		++emu->i8080.PC;								\
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
	uint8_t *byte = &emu->memory[(emu->i8080.H << 8) | emu->i8080.L];	// Points to byte at (HL).

	++(*byte);

	i8080_set_flag(&emu->i8080, FLAG_Z, *byte == 0x00);		// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, *byte & 0x80);		// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[*byte]);	// Parity Flag.

	++emu->i8080.PC;
}

/* Decrement Byte Instructions */

/* Macro for defining dcr instructions */
#define DCR_INSTR(r,R)	\
	INSTR(dcr_##r) {\
		--emu->i8080.R;	\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.R == 0x0);		\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.R & 0x80);		\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.R]);	\
		++emu->i8080.PC;							\
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
	uint8_t *byte = &emu->memory[(emu->i8080.H << 8) | emu->i8080.L]; // Points to the wanted byte.

	// TODO: know how the A flag should be set in a subtraction.

	--(*byte);

	i8080_set_flag(&emu->i8080, FLAG_Z, *byte == 0x00);		// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, *byte & 0x80);		// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[*byte]);	// Parity Flag.
	++emu->i8080.PC;
}

/* Add Byte Instructions */
#define ADD_INSTR(r,R)	\
	INSTR(add_##r) {\
		i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + emu->i8080.R) > 255);			\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(emu->i8080.R)) > 15);		\
		emu->i8080.A += emu->i8080.R;	\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);					\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);					\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);				\
		++emu->i8080.PC;										\
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
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte)) > 15);		// Auxiliary Carry Flag.

	emu->i8080.A += byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	++emu->i8080.PC;
}

INSTR(adi) {
	// Adds an immediat to A.
	uint8_t byte = get_byte_from_instruction(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + byte) > 255);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte)) > 15);		// Auxiliary Carry Flag.

	emu->i8080.A += byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	emu->i8080.PC += 2;
}

/* Add Byte with Carry-In Instructions */
#define ADC_INSTR(r,R)	\
	INSTR(adc_##r) {	\
		uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;						\
		i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + emu->i8080.R + carry) > 255);			\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN((emu->i8080.R + carry))) > 15);		\
		emu->i8080.A += emu->i8080.R + carry;									\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);						\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);						\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);					\
		++emu->i8080.PC;											\
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
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN((byte + carry))) > 15);		// Auxiliary Carry Flag.

	emu->i8080.A += byte + carry;
	
	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);					// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);					// Signal Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);				// Parity Flag.

	++emu->i8080.PC;
}

INSTR(aci) {
	// Adds an immediat to A and carry.
	uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;
	uint8_t byte = get_byte_from_instruction(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.A + byte + carry) > 255);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.A) + LN(byte)) > 15);		// Auxiliary Carry Flag.

	emu->i8080.A += byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	emu->i8080.PC += 2;
}

/* Sub Byte Instructions */
#define SUB_INSTR(r,R)	\
	INSTR(sub_##r) {	\
		i8080_set_flag(&emu->i8080, FLAG_C, emu->i8080.R > emu->i8080.A);		\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN(emu->i8080.R) > LN(emu->i8080.A)));	\
		emu->i8080.A -= emu->i8080.R;							\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);			\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);			\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);		\
		++emu->i8080.PC;								\
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

	i8080_set_flag(&emu->i8080, FLAG_C, byte > emu->i8080.A);				// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, LN(byte) > LN(emu->i8080.A));			// Auxiliary Carry Flag.

	emu->i8080.A -= byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	++emu->i8080.PC;
}

INSTR(sui) {
	uint8_t byte = get_byte_from_instruction(emu);
	
	i8080_set_flag(&emu->i8080, FLAG_C, byte > emu->i8080.A);
	i8080_set_flag(&emu->i8080, FLAG_A, (LN(byte) > LN(emu->i8080.A)));

	emu->i8080.A -= byte;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);

	emu->i8080.PC += 2;
}

/* Sub Byte with Borrow In Instructions */
#define SBB_INSTR(r,R)	\
	INSTR(sbb_##r) {\
		uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;				\
		i8080_set_flag(&emu->i8080, FLAG_C, (emu->i8080.R - carry) > emu->i8080.A);		\
		i8080_set_flag(&emu->i8080, FLAG_A, (LN((emu->i8080.R - carry)) > LN(emu->i8080.A)));	\
		emu->i8080.A -= emu->i8080.R - carry;							\
		i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				\
		i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				\
		i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			\
		++emu->i8080.PC;									\
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

	i8080_set_flag(&emu->i8080, FLAG_C, (byte - carry) > emu->i8080.A);			// Carry Flag.
	i8080_set_flag(&emu->i8080, FLAG_A, LN((byte - carry)) > LN(emu->i8080.A));		// Auxiliary Carry Flag.

	emu->i8080.A -= byte - carry;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);				// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);				// Signal FLag.
	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);			// Parity Flag.

	++emu->i8080.PC;
}

INSTR(sbi) {
	uint8_t carry = i8080_get_flag(&emu->i8080, FLAG_C) ? 1 : 0;
	uint8_t byte = get_byte_from_instruction(emu);

	i8080_set_flag(&emu->i8080, FLAG_C, (byte - carry) > emu->i8080.A);
	i8080_set_flag(&emu->i8080, FLAG_A, (LN((byte - carry)) > LN(emu->i8080.A)));

	emu->i8080.A -= byte - carry;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.A == 0x00);
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.A & 0x80);

	i8080_set_flag(&emu->i8080, FLAG_P, parity_table[emu->i8080.A]);
	emu->i8080.PC += 2;
}

