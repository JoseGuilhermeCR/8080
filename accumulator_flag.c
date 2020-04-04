#include "i8080emu.h"

/* Accumulator and Flag Instructions */
INSTR(cma) {
	emu->i8080.A = ~emu->i8080.A;
}

INSTR(stc) {
	// Carry = 1.
	i8080_set_flag(&emu->i8080, FLAG_C, true);
}

INSTR(cmc) {
	// Carry = !Carry.
	i8080_set_flag(&emu->i8080, FLAG_C, !i8080_get_flag(&emu->i8080, FLAG_C));
}