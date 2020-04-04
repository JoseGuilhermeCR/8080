#include "i8080emu.h"

/* Constrol Instructions */
INSTR(hlt) {
	// NOP; PC = PC - 1
	--emu->i8080.PC;
}