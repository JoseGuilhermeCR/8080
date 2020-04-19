#include "i8080emu.h"

/* Rotate Instructions */
INSTR(rlc) {
	// Rotate A to the left.
	uint8_t previous_bit7 = (emu->i8080.A & 0x80) >> 7;	// Save previous bit 7.

	emu->i8080.A = (emu->i8080.A << 1) | previous_bit7;	// Shift 1 bit to the left.
														// Bit 0 is set to previous bit 7.

	i8080_set_flag(&emu->i8080, FLAG_C, previous_bit7);	// Carry Flag.

	++emu->i8080.PC;

	return 4;
}

INSTR(rrc) {
	// Rotate A to the right.
	uint8_t previous_bit0 = emu->i8080.A & 0x01;		// Save previous bit 0.
	
	emu->i8080.A = (emu->i8080.A >> 1) | (previous_bit0 << 7); 	// Shift 1 bit to the right.
																// Bit 7 is set to previous bit 0.

	i8080_set_flag(&emu->i8080, FLAG_C, previous_bit0);	// Carry Flag.

	++emu->i8080.PC;

	return 4;
}

INSTR(ral) {
	// Rotate A to the left through Carry.
	uint8_t previous_cy = i8080_get_flag(&emu->i8080, FLAG_C) ? 0x01 : 0x00;	// Save previous carry flag.
	uint8_t previous_bit7 = emu->i8080.A & 0x80;	// Save previous bit 7.

	emu->i8080.A = (emu->i8080.A << 1) | previous_cy;	// Shift 1 bit to the left.
														// Bit 0 is set to previous carry flag.

	i8080_set_flag(&emu->i8080, FLAG_C, previous_bit7);	// Carry Flag is set to previous bit 7

	++emu->i8080.PC;

	return 4;
}

INSTR(rar) {
	// Rotate A to the right through Carry.
	uint8_t previous_cy = i8080_get_flag(&emu->i8080, FLAG_C) ? 0x01 : 0x00;
	uint8_t previous_bit0 = emu->i8080.A & 0x01;

	emu->i8080.A = (emu->i8080.A >> 1) | (previous_cy << 7);	// Shift 1 bit to the right, bit 7 is previous carry.
	
	i8080_set_flag(&emu->i8080, FLAG_C, previous_bit0);	// Carry Flag is set to previous bit 0

	++emu->i8080.PC;

	return 4;
}
