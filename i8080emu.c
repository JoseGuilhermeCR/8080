/* José Guilherme de C. Rodrigues - 03/2020 */
#include "i8080emu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	i8080emu *i = i8080emu_create();
	i8080emu_load_program_into_memory(i, "invaders/invaders");
	
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_destroy(i);

	return 0;
}

// Number of 1 bits in numbers from 0 to 0xff.
uint8_t parity_table[0x100] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

// NOPS will be substituted by a NULL.
void (*instruction_table[0x100]) (i8080emu *emu) = {
	NULL, lxi_b, stax_b, inx_b, inr_b, dcr_b, mvi_b, rlc,
	NULL, dad_b, ldax_b, dcx_b, inr_c, dcr_c, mvi_c, rrc,
	NULL, lxi_d, stax_d, 
};

/* Setup functions */
i8080emu *i8080emu_create() {
	i8080emu *emu = malloc(sizeof(i8080emu));

	// Set all registers to 0.
	emu->i8080.A = 0x00;
	emu->i8080.F = 0x00;
	emu->i8080.B = 0x00;
	emu->i8080.C = 0x00;
	emu->i8080.D = 0x00;
	emu->i8080.E = 0x00;
	emu->i8080.H = 0x00;
	emu->i8080.L = 0x00;
	emu->i8080.PC  = 0x0000; //0x0000;
	emu->i8080.SP  = 0x0000;

	// Allocate memory, for now 65536 bytes (max possible).
	emu->memory = malloc(sizeof(uint8_t) * 0x10000);
	memset(emu->memory, 0x00, 0x10000);

	return emu;
}

/* We may want to choose where in memory the program will be loaded. */
void i8080emu_load_program_into_memory(i8080emu *emu, const char *filename) {
	FILE *file = fopen(filename, "rb");

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	fread(emu->memory, size, 1, file);

	fclose(file);
}

void i8080emu_destroy(i8080emu *emu) {
	free(emu->memory);
	free(emu);
	emu = NULL;
}

/* Emulation functions */
bool i8080_get_flag(const i8080 *i8080, Flags flag) {
	return i8080->F & flag;
}

void i8080_set_flag(i8080 *i8080, Flags flag, bool value) {
	if (value)
		i8080->F |= flag;
	else
		i8080->F &= ~flag;
}

void i8080emu_cycle(i8080emu *emu) {
	if (instruction_table[emu->memory[emu->i8080.PC]])
		(*instruction_table[emu->memory[emu->i8080.PC]])(emu);
	++emu->i8080.PC;
}

/* 8 Bit Transfer Instructions */
INSTR(mvi_b) {
	// Places byte in register B.
	emu->i8080.B = emu->memory[++emu->i8080.PC];
}

INSTR(mvi_c) {
	// Places byte in register C.
	emu->i8080.C = emu->memory[++emu->i8080.PC];
}

INSTR(stax_b) {
	// Moves the value stored in A to memory at (BC).
	emu->memory[(emu->i8080.B << 8) | emu->i8080.C] = emu->i8080.A;
}

INSTR(stax_d) {
	// Moves the value stored in A to memory at (DE).
	emu->memory[(emu->i8080.D << 8) | emu->i8080.E] = emu->i8080.A;
}

INSTR(ldax_b) {
	// Moves the value from memory at (BC) to A.
	emu->i8080.A = emu->memory[++emu->i8080.PC];
}


/* 16 Bit Transfer Instructions */
INSTR(lxi_b) {
	// Places word in register pair BC.
	// Words are in little endian (lower byte should be in C and higher in B).
	emu->i8080.C = emu->memory[++emu->i8080.PC];
	emu->i8080.B = emu->memory[++emu->i8080.PC];
}

INSTR(lxi_d) {
	// Places word in register pair DE.
	// Words are in little endian (lower byte should be in E and higher in D).
	emu->i8080.E = emu->memory[++emu->i8080.PC];
	emu->i8080.D = emu->memory[++emu->i8080.PC];
}

/* Increment Byte Instructions */
INSTR(inr_b) {
	i8080_set_flag(&emu->i8080, FLAG_A, carry_out_lower_nibble(emu->i8080.B, 1));	// Auxiliary Carry Flag.

	// Increment register B.
	++emu->i8080.B;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.B == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.B & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.B));			// Parity Flag.
}

INSTR(inr_c) {
	i8080_set_flag(&emu->i8080, FLAG_A, carry_out_lower_nibble(emu->i8080.C, 1));	// Auxiliary Carry Flag.

	// Increment register C.
	++emu->i8080.C;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.C == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.C & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.C));			// Parity Flag.
}

/* Decrement Byte Instructions */
INSTR(dcr_b) {
	// TODO: know how the A flag should be set in a subtraction.
	// Decrement register B.
	--emu->i8080.B;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.B == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.B & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.B));			// Parity Flag.
}

INSTR(dcr_c) {
	// TODO: know how the A flag should be set in a subtraction.
	// Decrement register C.
	--emu->i8080.C;

	i8080_set_flag(&emu->i8080, FLAG_Z, emu->i8080.C == 0x00);			// Zero Flag.
	i8080_set_flag(&emu->i8080, FLAG_S, emu->i8080.C & 0x80);			// Sign Flag.
	i8080_set_flag(&emu->i8080, FLAG_P, even_parity(emu->i8080.C));			// Parity Flag.
}

/* Increment Register Pair Instructions */
INSTR(inx_b) {
	// Increment register pair BC
	uint16_t BC = ((emu->i8080.B << 8) | emu->i8080.C) + 1;
	emu->i8080.C = LB(BC);
	emu->i8080.B = HB(BC);
}

/* Decrement Register Pair Instructions */
INSTR(dcx_b) {
	// Decrement register pair BC.
	uint16_t BC = ((emu->i8080.B << 8) | emu->i8080.C) - 1;
	emu->i8080.C = LB(BC);
	emu->i8080.B = HB(BC);
}

/* Rotate Instructions */
INSTR(rlc) {
	// Rotate A to the left.
	uint8_t previous_bit7 = (emu->i8080.A & 0x80) >> 7;	// Save previous bit 7.

	emu->i8080.A <<= 1;					// Shift 1 bit to the left.

	emu->i8080.A |= previous_bit7;				// Bit 0 is set to previous bit 7.

	i8080_set_flag(&emu->i8080, FLAG_C, previous_bit7);	// Carry Flag.
}

INSTR(rrc) {
	// Rotate A to the right.
	uint8_t previous_bit0 = emu->i8080.A & 0x01;		// Save previous bit 0.
	
	emu->i8080.A >>= 1;					// Shift 1 bit to the right.
	
	emu->i8080.A |= previous_bit0 << 7;			// Bit 7 is set to previous bit 0.

	i8080_set_flag(&emu->i8080, FLAG_C, previous_bit0);	// Carry Flag.
}

/* Double Byte Add Instructions */
INSTR(dad_b) {
	// HL += BC;
	uint16_t HL = (emu->i8080.H << 8) | emu->i8080.L;	// Put together the values held by H and L.
	uint16_t BC = (emu->i8080.B << 8) | emu->i8080.C;	// Same with BC.

	HL += BC;
	
	// Put each byte in their own register.
	emu->i8080.L = LB(HL);
	emu->i8080.H = HB(HL);

	i8080_set_flag(&emu->i8080, FLAG_C, (HL + BC) > 65535);// Carry Flag.
}

/* Help */
bool carry_out_lower_nibble(uint8_t a, uint8_t b) {
	// Add the lower nibbles and check if bit 4 is set.
	// If it is, a carry out happened.
	return ((a & 0xf) + (b & 0xf)) & 0x10;
}

bool even_parity(uint8_t a) {
	// Return true if the number of 1 bits in a is even.
	return parity_table[a] % 2 == 0;
}

/* Debug Stuff */
void i8080emu_print_registers(i8080emu *emu) {
	printf(
		"A: %02x F: %02x\n"
		"B: %02x C: %02x\n"
		"D: %02x E: %02x\n"
		"H: %02x L: %02x\n"
		"SP: %04x\n"
		"PC: %04x\n",
		emu->i8080.A,  emu->i8080.F,
		emu->i8080.B,  emu->i8080.C,
		emu->i8080.D,  emu->i8080.E,
		emu->i8080.H,  emu->i8080.L,
		emu->i8080.SP,
		emu->i8080.PC
	);
}
