#include "i8080emu.h"

void return_from_subroutine(i8080emu *emu) {
	// Set's PC to the address contained at SP and SP+1.
	emu->i8080.PC = i8080emu_read_word_memory(emu, emu->i8080.SP);
	emu->i8080.SP += 2;

}

uint8_t return_with_condition(i8080emu *emu, Flags flag, bool value) {
	uint8_t cycles = 5;

	if (i8080emu_get_flag(emu, flag) == value) {
		return_from_subroutine(emu);
		cycles = 11;
	} else {
		++emu->i8080.PC;
	}

	return cycles;
}

void save_pc_in_stack(i8080emu *emu) {
	// Saves PC in stack.
	i8080emu_write_word_memory(emu, emu->i8080.SP - 2, emu->i8080.PC);
	emu->i8080.SP -= 2;
}

void call_addr(i8080emu *emu) {
	// Gets address from instruction.
	uint16_t addr = get_word_from_instruction(emu);

	// When any kind of return occurs, PC needs to be at the next instruction.
	emu->i8080.PC += 3;
	save_pc_in_stack(emu);

	// Set PC = addr
	emu->i8080.PC = addr;
}

uint8_t call_addr_with_condition(i8080emu *emu, Flags flag, bool value) {
	uint8_t cycles = 11;

	if (i8080emu_get_flag(emu, flag) == value) {
		call_addr(emu);
		cycles = 17;
	} else {
		emu->i8080.PC += 3;
	}

	return cycles;
}

void jump(i8080emu *emu) {
	emu->i8080.PC = get_word_from_instruction(emu);
}

uint8_t jump_with_condition(i8080emu *emu, Flags flag, bool value) {
	if (i8080emu_get_flag(emu, flag) == value)
		jump(emu);
	else
		emu->i8080.PC += 3;

	return 10;
}

/* Branch Control Instructions */

INSTR(pchl) {
	emu->i8080.PC = (emu->i8080.H << 8) | emu->i8080.L;

	return 5;
}

/* Returns */
INSTR(rnz) {
	return return_with_condition(emu, FLAG_Z, false);
}

INSTR(rz) {
	return return_with_condition(emu, FLAG_Z, true);
}

INSTR(ret) {
	return_from_subroutine(emu);

	return 10;
}

INSTR(rnc) {
	return return_with_condition(emu, FLAG_C, false);
}

INSTR(rc) {
	return return_with_condition(emu, FLAG_C, true);
}

INSTR(rpo) {
	return return_with_condition(emu, FLAG_P, false); 
}

INSTR(rpe) {
	return return_with_condition(emu, FLAG_P, true); 
}

INSTR(rp) {
	return return_with_condition(emu, FLAG_S, false);
}

INSTR(rm) {
	return return_with_condition(emu, FLAG_S, true);
}

/* Jumps */
INSTR(jnz) {
	return jump_with_condition(emu, FLAG_Z, false);
}

INSTR(jz) {
	return jump_with_condition(emu, FLAG_Z, true);
}

INSTR(jmp) {
	jump(emu);

	return 10;
}

INSTR(jnc) {
	return jump_with_condition(emu, FLAG_C, false);
}

INSTR(jc) {
	return jump_with_condition(emu, FLAG_C, true);
}

INSTR(jpo) {
	return jump_with_condition(emu, FLAG_P, false);
}

INSTR(jpe) {
	return jump_with_condition(emu, FLAG_P, true);
}

INSTR(jp) {
	return jump_with_condition(emu, FLAG_S, false);
}

INSTR(jm) {
	return jump_with_condition(emu, FLAG_S, true);
}
/* Calls */
INSTR(cnz) {
	return call_addr_with_condition(emu, FLAG_Z, false);
}

INSTR(cz) {
	return call_addr_with_condition(emu, FLAG_Z, true);
}

INSTR(call) {
	call_addr(emu);

	return 17;
}

INSTR(cnc) {
	return call_addr_with_condition(emu, FLAG_C, false);
}

INSTR(cc) {
	return call_addr_with_condition(emu, FLAG_C, true);
}

INSTR(cpo) {
	return call_addr_with_condition(emu, FLAG_P, false);
}

INSTR(cpe) {
	return call_addr_with_condition(emu, FLAG_P, true);
}

INSTR(cp) {
	return call_addr_with_condition(emu, FLAG_S, false);
}

INSTR(cm) {
	return call_addr_with_condition(emu, FLAG_S, true);
}

/* Stack Operation Instructions */

// Pops value from stack and put in register pair RR1.
#define POP_INSTR(r,R,R1)	\
	INSTR(pop_##r)	{	\
		emu->i8080.R1 = i8080emu_read_byte_memory(emu, emu->i8080.SP);		\
		emu->i8080.R = i8080emu_read_byte_memory(emu, emu->i8080.SP + 1);	\
		emu->i8080.SP += 2;							\
		++emu->i8080.PC;							\
		return 10;								\
	}

POP_INSTR(b,B,C)
POP_INSTR(d,D,E)
POP_INSTR(h,H,L)
POP_INSTR(psw,A,F)

// Pushes value from register pair RR1 into stack.
#define PUSH_INSTR(r,R,R1)	\
	INSTR(push_##r)	{	\
		i8080emu_write_byte_memory(emu, emu->i8080.SP - 2, emu->i8080.R1);	\
		i8080emu_write_byte_memory(emu, emu->i8080.SP - 1, emu->i8080.R);	\
		emu->i8080.SP -= 2;							\
		++emu->i8080.PC;							\
		return 11;								\
	}

PUSH_INSTR(b,B,C)
PUSH_INSTR(d,D,E)
PUSH_INSTR(h,H,L)
PUSH_INSTR(psw,A,F)

/* Interrupt specific calls */
void call_interrupt_subroutine(i8080emu *emu, uint8_t number) {
	// Advance PC to next instruction before pushing into the stack.
	++emu->i8080.PC;
	save_pc_in_stack(emu);
	
	// Calls the needed interrupt subroutine according to the rst instruction number.
	emu->i8080.PC = number * 0x0008;
}

#define RST_INSTR(n)	\
	INSTR(rst_##n) {\
		call_interrupt_subroutine(emu, n);	\
		return 11;				\
	}

RST_INSTR(0)
RST_INSTR(1)
RST_INSTR(2)
RST_INSTR(3)
RST_INSTR(4)
RST_INSTR(5)
RST_INSTR(6)
RST_INSTR(7)
