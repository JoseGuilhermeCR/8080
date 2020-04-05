#include "i8080emu.h"

/* 16 Bit Transfer Instructions */

/* Macro for defining lxi instructions */
#define LXI_INSTR(r,R,R1)	\
	INSTR(lxi_##r) {	\
		emu->i8080.R1 = emu->memory[++emu->i8080.PC];	\
		emu->i8080.R  = emu->memory[++emu->i8080.PC];	\
	}
		

LXI_INSTR(b,B,C)
LXI_INSTR(d,D,E)
LXI_INSTR(h,H,L)

/* Example with comments:
INSTR(lxi_d) {
	// Loads word in register pair DE.
	// Words are in little endian (lower byte should be in E and higher in D).
	emu->i8080.E = emu->memory[++emu->i8080.PC];
	emu->i8080.D = emu->memory[++emu->i8080.PC];
}
*/

INSTR(lxi_sp) {
	// Loads word in SP.
	emu->i8080.SP = emu->memory[++emu->i8080.PC];
	emu->i8080.SP |= emu->memory[++emu->i8080.PC] << 8;
}

INSTR(shld) {
	// Stores HL at addr, addr +1.
	// First, put address into a variable.
	uint16_t addr = emu->memory[++emu->i8080.PC];
	addr |= emu->memory[++emu->i8080.PC] << 8;

	// Places HL in addr, addr+1 (Little Endian here!)
	emu->memory[addr] = emu->i8080.L;
	emu->memory[addr + 1] = emu->i8080.H;	
}

INSTR(lhld) {
	// Loads a word from addr, addr+1 to HL
	// First, put address into a variable.
	uint16_t addr = emu->memory[++emu->i8080.PC];
	addr |= emu->memory[++emu->i8080.PC] << 8;

	// Loads the value from memory.
	emu->i8080.L = emu->memory[addr];
	emu->i8080.H = emu->memory[addr + 1];
}