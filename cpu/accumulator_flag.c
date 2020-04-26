#include "i8080emu.h"

/* Accumulator and Flag Instructions */
INSTR(cma) {
	emu->i8080.A = ~emu->i8080.A;
	++emu->i8080.PC;

	return 4;
}

INSTR(stc) {
	// Carry = 1.
	i8080emu_set_flag(emu, FLAG_C, true);
	++emu->i8080.PC;

	return 4;
}

INSTR(cmc) {
	// Carry = !Carry.
	i8080emu_set_flag(emu, FLAG_C, !i8080emu_get_flag(emu, FLAG_C));
	++emu->i8080.PC;

	return 4;
}
