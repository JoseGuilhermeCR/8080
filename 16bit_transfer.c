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
	// Places word in register pair DE.
	// Words are in little endian (lower byte should be in E and higher in D).
	emu->i8080.E = emu->memory[++emu->i8080.PC];
	emu->i8080.D = emu->memory[++emu->i8080.PC];
}
*/

INSTR(shld) {
	// Places HL at memory address given in the instruction.
	// Remember, this works in little endian!
	uint16_t addr = (emu->memory[emu->i8080.PC + 2] << 8) | emu->memory[emu->i8080.PC + 1];
	emu->memory[addr] = emu->i8080.L;
	emu->memory[addr + 1] = emu->i8080.H;	
}
