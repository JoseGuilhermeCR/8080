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
	NULL, lxi_d, stax_d, inx_d, inr_d, dcr_d, mvi_d, ral,
	NULL, dad_d, ldax_d, dcx_d, inr_e,
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
	emu->i8080.PC  = 0x1d0a; //0x0000;
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
