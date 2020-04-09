#include "i8080emu.h"

/* Constrol Instructions */
INSTR(nop) {
	++emu->i8080.PC;

	return 4;
}

INSTR(hlt) {
	// NOP; PC = PC - 1
	--emu->i8080.PC;

	return 7;
}
