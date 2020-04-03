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

/* Example with comments:
INSTR(lxi_d) {
	// Places word in register pair DE.
	// Words are in little endian (lower byte should be in E and higher in D).
	emu->i8080.E = emu->memory[++emu->i8080.PC];
	emu->i8080.D = emu->memory[++emu->i8080.PC];
}
*/
