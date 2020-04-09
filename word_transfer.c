#include "i8080emu.h"

/* 16 Bit Transfer Instructions */

/* Macro for defining lxi instructions */
#define LXI_INSTR(r,R,R1)	\
	INSTR(lxi_##r) {	\
		emu->i8080.R1 = emu->memory[emu->i8080.PC + 1];		\
		emu->i8080.R  = emu->memory[++emu->i8080.PC + 2]; 	\
		emu->i8080.PC += 3;					\
	}
		

LXI_INSTR(b,B,C)
LXI_INSTR(d,D,E)
LXI_INSTR(h,H,L)

INSTR(lxi_sp) {
	// Loads word in SP.
	emu->i8080.SP = get_word_from_instruction(emu);

	emu->i8080.PC += 3;
}

INSTR(shld) {
	// Stores HL at addr, addr +1.
	// First, put address into a variable.
	uint16_t addr = get_word_from_instruction(emu);

	// Places HL in addr, addr+1 (Little Endian here!)
	emu->memory[addr] = emu->i8080.L;
	emu->memory[addr + 1] = emu->i8080.H;

	emu->i8080.PC += 3;
}

INSTR(lhld) {
	// Loads a word from addr, addr+1 to HL
	// First, put address into a variable.
	uint16_t addr = get_word_from_instruction(emu);

	// Loads the value from memory.
	emu->i8080.L = emu->memory[addr];
	emu->i8080.H = emu->memory[addr + 1];

	emu->i8080.PC += 3;
}

INSTR(xthl) {
	// Swap HL with SP, SP+1
	uint8_t H = emu->i8080.H;
	uint8_t L = emu->i8080.L;

	emu->i8080.H = emu->memory[emu->i8080.SP + 1];
	emu->i8080.L = emu->memory[emu->i8080.SP];

	emu->memory[emu->i8080.SP + 1] = H;
	emu->memory[emu->i8080.SP] = L;

	++emu->i8080.PC;
}

INSTR(xchg) {
	// Swap HL with DE.
	uint8_t H = emu->i8080.H;
	uint8_t L = emu->i8080.L;

	emu->i8080.H = emu->i8080.D;
	emu->i8080.L = emu->i8080.E;

	emu->i8080.D = H;
	emu->i8080.E = L;

	++emu->i8080.PC;
}

INSTR(sphl) {
	// SP = HL.
	emu->i8080.SP = (emu->i8080.H << 8) | emu->i8080.L;
}
