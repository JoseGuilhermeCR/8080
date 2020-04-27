// José Guilherme de C. Rodrigues - 03/2020
#include "i8080emu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1 for numbers with even number of 1 bits and 0 for numbers with odd number of 1 bits.
const uint8_t parity_table[0x100] = {
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

// Original NOP is instruction 0. There are some alternative opcodes in which I used nop. This
// is probably fine since they shouldn't even be used.
const uint8_t (*instruction_table[0x100]) (i8080emu *const emu) = {
	nop, lxi_b, stax_b, inx_b, inr_b, dcr_b, mvi_b, rlc,
	nop, dad_b, ldax_b, dcx_b, inr_c, dcr_c, mvi_c, rrc,
	nop, lxi_d, stax_d, inx_d, inr_d, dcr_d, mvi_d, ral,
	nop, dad_d, ldax_d, dcx_d, inr_e, dcr_e, mvi_e, rar,
	nop, lxi_h, shld, inx_h, inr_h, dcr_h, mvi_h, daa,
	nop, dad_h, lhld, dcx_h, inr_l, dcr_l, mvi_l, cma,
	nop, lxi_sp, sta, inx_sp, inr_m, dcr_m, mvi_m, stc,
	nop, dad_sp, lda, dcx_sp, inr_a, dcr_a, mvi_a, cmc,
	mov_bb, mov_bc, mov_bd, mov_be, mov_bh, mov_bl, mov_bm, mov_ba,
	mov_cb, mov_cc, mov_cd, mov_ce, mov_ch, mov_cl, mov_cm, mov_ca,
	mov_db, mov_dc, mov_dd, mov_de, mov_dh, mov_dl, mov_dm, mov_da,
	mov_eb, mov_ec, mov_ed, mov_ee, mov_eh, mov_el, mov_em, mov_ea,
	mov_hb, mov_hc, mov_hd, mov_he, mov_hh, mov_hl, mov_hm, mov_ha,
	mov_lb, mov_lc, mov_ld, mov_le, mov_lh, mov_ll, mov_lm, mov_la,
	mov_mb, mov_mc, mov_md, mov_me, mov_mh, mov_ml, hlt, mov_ma,
	mov_ab, mov_ac, mov_ad, mov_ae, mov_ah, mov_al, mov_am, mov_aa,
	add_b, add_c, add_d, add_e, add_h, add_l, add_m, add_a,
	adc_b, adc_c, adc_d, adc_e, adc_h, adc_l, adc_m, adc_a,
	sub_b, sub_c, sub_d, sub_e, sub_h, sub_l, sub_m, sub_a,
	sbb_b, sbb_c, sbb_d, sbb_e, sbb_h, sbb_l, sbb_m, sbb_a,
	ana_b, ana_c, ana_d, ana_e, ana_h, ana_l, ana_m, ana_a,
	xra_b, xra_c, xra_d, xra_e, xra_h, xra_l, xra_m, xra_a,
	ora_b, ora_c, ora_d, ora_e, ora_h, ora_l, ora_m, ora_a,
	cmp_b, cmp_c, cmp_d, cmp_e, cmp_h, cmp_l, cmp_m, cmp_a,
	rnz, pop_b, jnz, jmp, cnz, push_b, adi, rst_0,
	rz, ret, jz, nop, cz, call, aci, rst_1,
	rnc, pop_d, jnc, NULL /* OUT */, cnc, push_d, sui, rst_2,
	rc, nop, jc, NULL /* IN */, cc, nop, sbi, rst_3,
	rpo, pop_h, jpo, xthl, cpo, push_h, ani, rst_4,
	rpe, pchl, jpe, xchg, cpe, nop, xri, rst_5,
	rp, pop_psw, jp, di, cp, push_psw, ori, rst_6,
	rm, sphl, jm, ei, cm, nop, cpi, rst_7
};

// Setup functions
i8080emu *i8080emu_create() {
	i8080emu *emu = malloc(sizeof(i8080emu));

	// Set all registers to 0.
	emu->i8080.A = 0x00;
	emu->i8080.B = 0x00;
	emu->i8080.C = 0x00;
	emu->i8080.D = 0x00;
	emu->i8080.E = 0x00;
	emu->i8080.H = 0x00;
	emu->i8080.L = 0x00;
	emu->i8080.PC  = 0x0000;
	emu->i8080.SP  = 0x0000;

	// Correct starting value for flags register.
	emu->i8080.F = 0x02; 

	emu->i8080.INTE = 1;	// Interrupt system starts activated.

	// Allocate memory, for now 65536 bytes (max possible).
	emu->memory = malloc(sizeof(uint8_t) * 0x10000);
	memset(emu->memory, 0x00, 0x10000);

	return emu;
}

// We may want to choose where in memory the program will be loaded.
void i8080emu_load_program_into_memory(i8080emu *const emu, const char *filename, uint16_t offset, bool print_info) {
	FILE *file = fopen(filename, "rb");

	if (file) {
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		fseek(file, 0, SEEK_SET);

		fread(emu->memory + offset, size, 1, file);

		fclose(file);

		if (print_info) 
			printf("Loaded file %s with size %li and offset %u.\n",  filename, size, offset);
	} else {
		fprintf(stderr, "Couldn't load file %s.\n", filename);
	}
}

void i8080emu_destroy(i8080emu *emu) {
	free(emu->memory);
	free(emu);
	emu = NULL;
}

// Emulation functions
bool i8080emu_get_flag(const i8080emu *const emu, Flags flag) {
	return emu->i8080.F & flag;
}

void i8080emu_set_flag(i8080emu *const emu, Flags flag, bool value) {
	if (value)
		emu->i8080.F |= flag;
	else
		emu->i8080.F &= ~flag;

	emu->i8080.F = (emu->i8080.F & 0xD7) | 0x02;
}

unsigned i8080emu_run_cycles(i8080emu *const emu, unsigned cycles) {
	unsigned done_cycles = 0;

	while (done_cycles < cycles) {
		if (instruction_table[emu->memory[emu->i8080.PC]]) {
			done_cycles += (*instruction_table[emu->memory[emu->i8080.PC]])(emu);
		} else {
			//printf("Instruction %02x not implemented yet.\n", emu->memory[emu->i8080.PC]);
			++emu->i8080.PC;
		}
	}

	return done_cycles;
}

// Memory manipulation
void i8080emu_write_byte_memory(i8080emu *const emu, uint16_t addr, uint8_t byte) {
	emu->memory[addr] = byte;
}

void i8080emu_write_word_memory(i8080emu *const emu, uint16_t addr, uint16_t word) {
	emu->memory[addr] = LB(word);
	emu->memory[addr + 1] = HB(word);
}

uint8_t i8080emu_read_byte_memory(const i8080emu *const emu, uint16_t addr) {
	return emu->memory[addr];
}

uint16_t i8080emu_read_word_memory(const i8080emu *const emu, uint16_t addr) {
	return ((emu->memory[addr + 1] << 8) | emu->memory[addr]);
}

// Help 
uint8_t get_byte_hl(const i8080emu *const emu) {
	return i8080emu_read_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L);
}

uint16_t get_word_from_instruction(const i8080emu *const emu) {
	return i8080emu_read_word_memory(emu, emu->i8080.PC + 1);
}

uint8_t get_byte_from_instruction(const i8080emu *const emu) {
	return i8080emu_read_byte_memory(emu, emu->i8080.PC + 1);
}

void set_zsp(i8080emu *const emu, uint8_t val) {
	i8080emu_set_flag(emu, FLAG_Z, val == 0x00);
	i8080emu_set_flag(emu, FLAG_S, val & 0x80);
	i8080emu_set_flag(emu, FLAG_P, parity_table[val]);
}

// Debug Stuff
void i8080emu_print_registers(const i8080emu *const emu) {
	printf(
		"A: %02x F: %02x\n"
		"B: %02x C: %02x\n"
		"D: %02x E: %02x\n"
		"H: %02x L: %02x\n"
		"SP: %04x\n"
		"PC: %04x\n"
		"S: %u Z: %u A: %u P: %u C: %u\n",
		emu->i8080.A,  emu->i8080.F,
		emu->i8080.B,  emu->i8080.C,
		emu->i8080.D,  emu->i8080.E,
		emu->i8080.H,  emu->i8080.L,
		emu->i8080.SP,
		emu->i8080.PC,
		i8080emu_get_flag(emu, FLAG_S),
		i8080emu_get_flag(emu, FLAG_Z),
		i8080emu_get_flag(emu, FLAG_A),
		i8080emu_get_flag(emu, FLAG_P),
		i8080emu_get_flag(emu, FLAG_C)
	);
}

// All instructions

// A few generic implementations that will help us not repeat that much code.
bool i8080emu_carry(int bit_no, uint8_t a, uint8_t b, bool cy) {
	uint16_t result = a + b + cy;
	uint16_t carry = result ^ a ^ b;
	return carry & (1 << bit_no);
}

void i8080emu_add(i8080emu *const emu, uint8_t *reg, uint8_t a, uint8_t cy) {
	const uint8_t result = *reg + a + cy;

	i8080emu_set_flag(emu, FLAG_C, i8080emu_carry(8, *reg, a, cy));
	i8080emu_set_flag(emu, FLAG_A, i8080emu_carry(4, *reg, a, cy));

	set_zsp(emu, result);

	*reg = result;
}

void i8080emu_sub(i8080emu *const emu, uint8_t *reg, uint8_t a, uint8_t cy) {
	i8080emu_add(emu, reg, ~a, (cy == 0) ? 1 : 0);
	i8080emu_set_flag(emu, FLAG_C, !i8080emu_get_flag(emu, FLAG_C));
}

void set_xor_or_flags(i8080emu *const emu) {
	i8080emu_set_flag(emu, FLAG_C, false);
	i8080emu_set_flag(emu, FLAG_A, false);
	set_zsp(emu, emu->i8080.A);
}

void set_cmp_flags(i8080emu *const emu, uint8_t value) {
	const uint16_t result = emu->i8080.A - value;
	i8080emu_set_flag(emu, FLAG_C, result >> 8);
	i8080emu_set_flag(emu, FLAG_A, ~(emu->i8080.A ^ result ^ value) & 0x10);
	set_zsp(emu, result & 0xff);
}

void i8080emu_and(i8080emu *const emu, uint8_t value) {
	uint8_t result = emu->i8080.A & value;
	i8080emu_set_flag(emu, FLAG_C, false);
	i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.A | value) & 0x08) != 0);
	set_zsp(emu, result);
	emu->i8080.A = result;
}

void i8080emu_or(i8080emu *const emu, uint8_t value) {
	emu->i8080.A |= value;
	set_xor_or_flags(emu);
}

void i8080emu_xor(i8080emu *const emu, uint8_t value) {
	emu->i8080.A ^= value;
	set_xor_or_flags(emu);
}

void return_from_subroutine(i8080emu *const emu) {
	// Set's PC to the address contained at SP and SP+1.
	emu->i8080.PC = i8080emu_read_word_memory(emu, emu->i8080.SP);
	emu->i8080.SP += 2;

}

uint8_t return_with_condition(i8080emu *const emu, Flags flag, bool value) {
	uint8_t cycles = 5;

	if (i8080emu_get_flag(emu, flag) == value) {
		return_from_subroutine(emu);
		cycles = 11;
	} else {
		++emu->i8080.PC;
	}

	return cycles;
}

void save_pc_in_stack(i8080emu *const emu) {
	// Saves PC in stack.
	i8080emu_write_word_memory(emu, emu->i8080.SP - 2, emu->i8080.PC);
	emu->i8080.SP -= 2;
}

void call_addr(i8080emu *const emu) {
	// Gets address from instruction.
	uint16_t addr = get_word_from_instruction(emu);

	// When any kind of return occurs, PC needs to be at the next instruction.
	emu->i8080.PC += 3;
	save_pc_in_stack(emu);

	// Set PC = addr
	emu->i8080.PC = addr;
}

uint8_t call_addr_with_condition(i8080emu *const emu, Flags flag, bool value) {
	uint8_t cycles = 11;

	if (i8080emu_get_flag(emu, flag) == value) {
		call_addr(emu);
		cycles = 17;
	} else {
		emu->i8080.PC += 3;
	}

	return cycles;
}

void jump(i8080emu *const emu) {
	emu->i8080.PC = get_word_from_instruction(emu);
}

uint8_t jump_with_condition(i8080emu *const emu, Flags flag, bool value) {
	if (i8080emu_get_flag(emu, flag) == value)
		jump(emu);
	else
		emu->i8080.PC += 3;

	return 10;
}

// All Instructions
// Accumulator and Flag Instructions
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

// Branch Control Instructions
INSTR(pchl) {
	emu->i8080.PC = (emu->i8080.H << 8) | emu->i8080.L;

	return 5;
}

// Returns 
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

// Jumps
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

// Calls
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

// Stack Operation Instructions

// Pops value from stack and put in register pair RR1.
#define POP_INSTR(r,R,R1)\
	INSTR(pop_##r)	{\
		emu->i8080.R1 = i8080emu_read_byte_memory(emu, emu->i8080.SP);\
		emu->i8080.R = i8080emu_read_byte_memory(emu, emu->i8080.SP + 1);\
		emu->i8080.SP += 2;\
		++emu->i8080.PC;\
		return 10;\
	}

POP_INSTR(b,B,C)
POP_INSTR(d,D,E)
POP_INSTR(h,H,L)

INSTR(pop_psw) {
	emu->i8080.F = i8080emu_read_byte_memory(emu, emu->i8080.SP);
	emu->i8080.A = i8080emu_read_byte_memory(emu, emu->i8080.SP + 1);

	// F must not have the unused bits changed! They need to remain with the correct values.
	emu->i8080.F = (emu->i8080.F & 0xD7) | 0x02;

	emu->i8080.SP += 2;
	++emu->i8080.PC;
	return 10;
}

// Pushes value from register pair RR1 into stack.
#define PUSH_INSTR(r,R,R1)\
	INSTR(push_##r)	{\
		i8080emu_write_byte_memory(emu, emu->i8080.SP - 2, emu->i8080.R1);\
		i8080emu_write_byte_memory(emu, emu->i8080.SP - 1, emu->i8080.R);\
		emu->i8080.SP -= 2;\
		++emu->i8080.PC;\
		return 11;\
	}

PUSH_INSTR(b,B,C)
PUSH_INSTR(d,D,E)
PUSH_INSTR(h,H,L)
PUSH_INSTR(psw,A,F)

// Interrupt specific calls
void call_interrupt_subroutine(i8080emu *const emu, uint8_t number) {
	// Advance PC to next instruction before pushing into the stack.
	++emu->i8080.PC;
	save_pc_in_stack(emu);
	
	// Calls the needed interrupt subroutine according to the rst instruction number.
	emu->i8080.PC = number * 0x0008;
}

#define RST_INSTR(n)\
	INSTR(rst_##n) {\
		call_interrupt_subroutine(emu, n);\
		return 11;\
	}

RST_INSTR(0)
RST_INSTR(1)
RST_INSTR(2)
RST_INSTR(3)
RST_INSTR(4)
RST_INSTR(5)
RST_INSTR(6)
RST_INSTR(7)

// Logical And Instructions
#define ANA_INSTR(r,R)\
	INSTR(ana_##r) {\
		i8080emu_and(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 4;\
	}


ANA_INSTR(b,B)
ANA_INSTR(c,C)
ANA_INSTR(d,D)
ANA_INSTR(e,E)
ANA_INSTR(h,H)
ANA_INSTR(l,L)
ANA_INSTR(a,A)

INSTR(ana_m) {
	i8080emu_and(emu, get_byte_hl(emu));
	
	++emu->i8080.PC;
	return 7;
}

INSTR(ani) {
	i8080emu_and(emu, get_byte_from_instruction(emu));

	emu->i8080.PC += 2;
	return 7;
}

// Logical Xor Instructions
#define XRA_INSTR(r,R)\
	INSTR(xra_##r) {\
		i8080emu_xor(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 4;\
	}

XRA_INSTR(b,B)
XRA_INSTR(c,C)
XRA_INSTR(d,D)
XRA_INSTR(e,E)
XRA_INSTR(h,H)
XRA_INSTR(l,L)
XRA_INSTR(a,A)

INSTR(xra_m) {
	i8080emu_xor(emu, get_byte_hl(emu));

	++emu->i8080.PC;
	return 7;
}

INSTR(xri) {
	i8080emu_xor(emu, get_byte_from_instruction(emu));

	emu->i8080.PC += 2;
	return 7;
}

// Logical Or Instructions
#define ORA_INSTR(r,R)\
	INSTR(ora_##r) {\
		i8080emu_or(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 4;\
	}

ORA_INSTR(b,B)
ORA_INSTR(c,C)
ORA_INSTR(d,D)
ORA_INSTR(e,E)
ORA_INSTR(h,H)
ORA_INSTR(l,L)
ORA_INSTR(a,A)

INSTR(ora_m) {
	i8080emu_or(emu, get_byte_hl(emu));
	
	++emu->i8080.PC;
	return 7;
}

INSTR(ori) {
	i8080emu_or(emu, get_byte_from_instruction(emu));
	
	emu->i8080.PC += 2;
	return 7;
}

// Logical Compare Instructions
#define CMP_INSTR(r,R)\
	INSTR(cmp_##r) {\
		set_cmp_flags(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 4;\
	}

CMP_INSTR(b,B)
CMP_INSTR(c,C)
CMP_INSTR(d,D)
CMP_INSTR(e,E)
CMP_INSTR(h,H)
CMP_INSTR(l,L)
CMP_INSTR(a,A)

INSTR(cmp_m) {
	set_cmp_flags(emu, get_byte_hl(emu));
	
	++emu->i8080.PC;
	return 7;
}

INSTR(cpi) {
	set_cmp_flags(emu, get_byte_from_instruction(emu));
	
	emu->i8080.PC += 2;
	return 7;
}

// Increment byte instructions
// Macro for defining the inr instruction.
#define INR_INSTR(r,R)\
	INSTR(inr_##r) {\
		i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.R ^ 0x1) & 0x10) != ((emu->i8080.R + 0x1) & 0x10));\
		++emu->i8080.R;\
		set_zsp(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 5;\
	}

// Increment Byte Instructions
INR_INSTR(b,B)
INR_INSTR(c,C)
INR_INSTR(d,D)
INR_INSTR(e,E)
INR_INSTR(h,H)
INR_INSTR(l,L)
INR_INSTR(a,A)

INSTR(inr_m) {
	// Increments byte at (HL).
	uint8_t byte = get_byte_hl(emu);	// Get byte from memory
	i8080emu_set_flag(emu, FLAG_A, ((byte ^ 0x1) & 0x10) != ((byte + 0x1) & 0x10));

	++byte;
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, byte);	// Save incremented byte.

	set_zsp(emu, byte);
	++emu->i8080.PC;

	return 10;
}

// Decrement Byte Instructions

// Macro for defining dcr instructions
#define DCR_INSTR(r,R)\
	INSTR(dcr_##r) {\
		i8080emu_set_flag(emu, FLAG_A, ((emu->i8080.R ^ 0xff) & 0x10) != ((emu->i8080.R + 0xff) & 0x10));\
		--emu->i8080.R;\
		set_zsp(emu, emu->i8080.R);\
		++emu->i8080.PC;\
		return 5;\
	}

DCR_INSTR(b,B)
DCR_INSTR(c,C)
DCR_INSTR(d,D)
DCR_INSTR(e,E)
DCR_INSTR(h,H)
DCR_INSTR(l,L)
DCR_INSTR(a,A)

INSTR(dcr_m) {
	// Decrements byte at (HL).
	uint8_t byte = get_byte_hl(emu);	// Get byte from memory
	i8080emu_set_flag(emu, FLAG_A, ((byte ^ 0xff) & 0x10) != ((byte + 0xff) & 0x10));

	--byte;
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, byte);	// Save incremented byte.

	set_zsp(emu, byte);
	++emu->i8080.PC;

	return 10;
}

// Add Byte Instructions
#define ADD_INSTR(r,R)\
	INSTR(add_##r) {\
		i8080emu_add(emu, &emu->i8080.A, emu->i8080.R, 0);\
		++emu->i8080.PC;\
		return 4;\
	}

ADD_INSTR(b,B)
ADD_INSTR(c,C)
ADD_INSTR(d,D)
ADD_INSTR(e,E)
ADD_INSTR(h,H)
ADD_INSTR(l,L)
ADD_INSTR(a,A)

INSTR(add_m) {
	i8080emu_add(emu, &emu->i8080.A, get_byte_hl(emu), 0);
	++emu->i8080.PC;
	return 7;
}

INSTR(adi) {
	// Adds an immediat to A.
	i8080emu_add(emu, &emu->i8080.A, get_byte_from_instruction(emu), 0);
	emu->i8080.PC += 2;
	return 7;
}

// Add Byte with Carry-In Instructions
#define ADC_INSTR(r,R)\
	INSTR(adc_##r) {\
		i8080emu_add(emu, &emu->i8080.A, emu->i8080.R, i8080emu_get_flag(emu, FLAG_C));\
		++emu->i8080.PC;\
		return 4;\
	}

ADC_INSTR(b,B)
ADC_INSTR(c,C)
ADC_INSTR(d,D)
ADC_INSTR(e,E)
ADC_INSTR(h,H)
ADC_INSTR(l,L)
ADC_INSTR(a,A)

INSTR(adc_m) {
	i8080emu_add(emu, &emu->i8080.A, get_byte_hl(emu), i8080emu_get_flag(emu, FLAG_C));
	++emu->i8080.PC;
	return 7;
}

INSTR(aci) {
	// Adds an immediat to A and carry.
	i8080emu_add(emu, &emu->i8080.A, get_byte_from_instruction(emu), i8080emu_get_flag(emu, FLAG_C));
	emu->i8080.PC += 2;
	return 7;
}

// Sub Byte Instructions
#define SUB_INSTR(r,R)\
	INSTR(sub_##r) {\
		i8080emu_sub(emu, &emu->i8080.A, emu->i8080.R, 0);\
		++emu->i8080.PC;\
		return 4;\
	}

SUB_INSTR(b,B)
SUB_INSTR(c,C)
SUB_INSTR(d,D)
SUB_INSTR(e,E)
SUB_INSTR(h,H)
SUB_INSTR(l,L)
SUB_INSTR(a,A)

INSTR(sub_m) {
	i8080emu_sub(emu, &emu->i8080.A, get_byte_hl(emu), 0);
	++emu->i8080.PC;
	return 7;
}

INSTR(sui) {
	i8080emu_sub(emu, &emu->i8080.A, get_byte_from_instruction(emu), 0);
	emu->i8080.PC += 2;
	return 7;
}

// Sub Byte with Borrow In Instructions
#define SBB_INSTR(r,R)\
	INSTR(sbb_##r) {\
		i8080emu_sub(emu, &emu->i8080.A, emu->i8080.R, i8080emu_get_flag(emu, FLAG_C));\
		++emu->i8080.PC;\
		return 4;\
	}

SBB_INSTR(b,B)
SBB_INSTR(c,C)
SBB_INSTR(d,D)
SBB_INSTR(e,E)
SBB_INSTR(h,H)
SBB_INSTR(l,L)
SBB_INSTR(a,A)

INSTR(sbb_m) {
	i8080emu_sub(emu, &emu->i8080.A, get_byte_hl(emu), i8080emu_get_flag(emu, FLAG_C));
	++emu->i8080.PC;
	return 7;
}

INSTR(sbi) {
	i8080emu_sub(emu, &emu->i8080.A, get_byte_from_instruction(emu), i8080emu_get_flag(emu, FLAG_C));
	emu->i8080.PC += 2;
	return 7;
}

INSTR(daa) {
	bool cy = i8080emu_get_flag(emu, FLAG_C);
	uint8_t correction = 0;

	if (LN(emu->i8080.A) > 9 || i8080emu_get_flag(emu, FLAG_A)) {
		correction += 6;
	}

	if (HN(emu->i8080.A) > 9 || (HN(emu->i8080.A) >= 9 && LN(emu->i8080.A) > 9) || cy) {
		correction += 96;
		cy = true;
	}

	i8080emu_add(emu, &emu->i8080.A, correction, 0);
	i8080emu_set_flag(emu, FLAG_C, cy);

	++emu->i8080.PC;

	return 4;
}

// 8bit transfer instructions

// Macro for defining the mvi instructions
#define MVI_INSTR(r,R)\
	INSTR(mvi_##r) {\
		emu->i8080.R = get_byte_from_instruction(emu);\
		emu->i8080.PC += 2;\
		return 7;\
	}

MVI_INSTR(b,B)
MVI_INSTR(c,C)
MVI_INSTR(d,D)
MVI_INSTR(e,E)
MVI_INSTR(h,H)
MVI_INSTR(l,L)
MVI_INSTR(a,A)

INSTR(mvi_m) {
	// Stores byte in (HL).
	i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, get_byte_from_instruction(emu));
	emu->i8080.PC += 2;

	return 10;
}

// Macro for defining the stax instructions
#define STAX_INSTR(r,R,R1)\
	INSTR(stax_##r) {\
		i8080emu_write_byte_memory(emu, (emu->i8080.R << 8) | emu->i8080.R1, emu->i8080.A);	\
		++emu->i8080.PC;\
		return 7;\
	}

STAX_INSTR(b,B,C)
STAX_INSTR(d,D,E)

INSTR(sta) {
	// Stores A in addr.
	i8080emu_write_byte_memory(emu, get_word_from_instruction(emu), emu->i8080.A);
	emu->i8080.PC += 3;

	return 13;
}

// Macro for defining the ldax instructions
#define LDAX_INSTR(r,R,R1)\
	INSTR(ldax_##r)	{\
		emu->i8080.A = i8080emu_read_byte_memory(emu, (emu->i8080.R << 8) | emu->i8080.R1);\
		++emu->i8080.PC;\
		return 7;\
	}

LDAX_INSTR(b,B,C)
LDAX_INSTR(d,D,E)

INSTR(lda) {
	// Loads byte in (word) to A.
	emu->i8080.A = i8080emu_read_byte_memory(emu, get_word_from_instruction(emu));
	emu->i8080.PC += 3;

	return 13;
}

#define MOV_INSTR(r,r1,R,R1)\
	INSTR(mov_##r##r1) {\
		emu->i8080.R = emu->i8080.R1;\
		++emu->i8080.PC;\
		return 5;\
	}

#define MOV_RM_INSTR(r,r1,R)\
	INSTR(mov_##r##r1) {\
		emu->i8080.R = get_byte_hl(emu);\
		++emu->i8080.PC;\
		return 7;\
	}

#define MOV_MR_INSTR(r,r1,R)\
	INSTR(mov_##r##r1) {\
		i8080emu_write_byte_memory(emu, (emu->i8080.H << 8) | emu->i8080.L, emu->i8080.R);\
		++emu->i8080.PC;\
		return 7;\
	}

MOV_INSTR(b,b,B,B)
MOV_INSTR(b,c,B,C)
MOV_INSTR(b,d,B,D)
MOV_INSTR(b,e,B,E)
MOV_INSTR(b,h,B,H)
MOV_INSTR(b,l,B,L)
MOV_RM_INSTR(b,m,B)
MOV_INSTR(b,a,B,A)

MOV_INSTR(c,b,C,B)
MOV_INSTR(c,c,C,C)
MOV_INSTR(c,d,C,D)
MOV_INSTR(c,e,C,E)
MOV_INSTR(c,h,C,H)
MOV_INSTR(c,l,C,L)
MOV_RM_INSTR(c,m,C)
MOV_INSTR(c,a,C,A)

MOV_INSTR(d,b,D,B)
MOV_INSTR(d,c,D,C)
MOV_INSTR(d,d,D,D)
MOV_INSTR(d,e,D,E)
MOV_INSTR(d,h,D,H)
MOV_INSTR(d,l,D,L)
MOV_RM_INSTR(d,m,D)
MOV_INSTR(d,a,D,A)

MOV_INSTR(e,b,E,B)
MOV_INSTR(e,c,E,C)
MOV_INSTR(e,d,E,D)
MOV_INSTR(e,e,E,E)
MOV_INSTR(e,h,E,H)
MOV_INSTR(e,l,E,L)
MOV_RM_INSTR(e,m,E)
MOV_INSTR(e,a,E,A)

MOV_INSTR(h,b,H,B)
MOV_INSTR(h,c,H,C)
MOV_INSTR(h,d,H,D)
MOV_INSTR(h,e,H,E)
MOV_INSTR(h,h,H,H)
MOV_INSTR(h,l,H,L)
MOV_RM_INSTR(h,m,H)
MOV_INSTR(h,a,H,A)

MOV_INSTR(l,b,L,B)
MOV_INSTR(l,c,L,C)
MOV_INSTR(l,d,L,D)
MOV_INSTR(l,e,L,E)
MOV_INSTR(l,h,L,H)
MOV_INSTR(l,l,L,L)
MOV_RM_INSTR(l,m,L)
MOV_INSTR(l,a,L,A)

MOV_MR_INSTR(m,b,B)
MOV_MR_INSTR(m,c,C)
MOV_MR_INSTR(m,d,D)
MOV_MR_INSTR(m,e,E)
MOV_MR_INSTR(m,h,H)
MOV_MR_INSTR(m,l,L)
MOV_MR_INSTR(m,a,A)

MOV_INSTR(a,b,A,B)
MOV_INSTR(a,c,A,C)
MOV_INSTR(a,d,A,D)
MOV_INSTR(a,e,A,E)
MOV_INSTR(a,h,A,H)
MOV_INSTR(a,l,A,L)
MOV_RM_INSTR(a,m,A)
MOV_INSTR(a,a,A,A)

// Constrol Instructions
INSTR(nop) {
	++emu->i8080.PC;

	return 4;
}

INSTR(hlt) {
	// NOP; PC = PC - 1
	--emu->i8080.PC;

	return 7;
}

INSTR(ei) {
	// Enables interrupt.
	emu->i8080.INTE = 1;

	++emu->i8080.PC;

	return 4;
}

INSTR(di) {
	// Disables interrupt.
	emu->i8080.INTE = 0;

	++emu->i8080.PC;

	return 4;
}

// Rotate Instructions
INSTR(rlc) {
	// Rotate A to the left.
	uint8_t previous_bit7 = (emu->i8080.A & 0x80) >> 7;	// Save previous bit 7.

	emu->i8080.A = (emu->i8080.A << 1) | previous_bit7;	// Shift 1 bit to the left.
														// Bit 0 is set to previous bit 7.

	i8080emu_set_flag(emu, FLAG_C, previous_bit7);	// Carry Flag.

	++emu->i8080.PC;

	return 4;
}

INSTR(rrc) {
	// Rotate A to the right.
	uint8_t previous_bit0 = emu->i8080.A & 0x01;		// Save previous bit 0.
	
	emu->i8080.A = (emu->i8080.A >> 1) | (previous_bit0 << 7); 	// Shift 1 bit to the right.
																// Bit 7 is set to previous bit 0.

	i8080emu_set_flag(emu, FLAG_C, previous_bit0);	// Carry Flag.

	++emu->i8080.PC;

	return 4;
}

INSTR(ral) {
	// Rotate A to the left through Carry.
	uint8_t previous_cy = i8080emu_get_flag(emu, FLAG_C) ? 0x01 : 0x00;	// Save previous carry flag.
	uint8_t previous_bit7 = emu->i8080.A & 0x80;	// Save previous bit 7.

	emu->i8080.A = (emu->i8080.A << 1) | previous_cy;	// Shift 1 bit to the left.
														// Bit 0 is set to previous carry flag.

	i8080emu_set_flag(emu, FLAG_C, previous_bit7);	// Carry Flag is set to previous bit 7

	++emu->i8080.PC;

	return 4;
}

INSTR(rar) {
	// Rotate A to the right through Carry.
	uint8_t previous_cy = i8080emu_get_flag(emu, FLAG_C) ? 0x01 : 0x00;
	uint8_t previous_bit0 = emu->i8080.A & 0x01;

	emu->i8080.A = (emu->i8080.A >> 1) | (previous_cy << 7);	// Shift 1 bit to the right, bit 7 is previous carry.
	
	i8080emu_set_flag(emu, FLAG_C, previous_bit0);	// Carry Flag is set to previous bit 0

	++emu->i8080.PC;

	return 4;
}

// Increment Word Instructions

// Macro for defining inx instructions
#define INX_INSTR(r,R,R1)\
	INSTR(inx_##r) {\
		uint16_t register_pair = ((emu->i8080.R << 8) | emu->i8080.R1) + 1;\
		emu->i8080.R1 = LB(register_pair);\
		emu->i8080.R  = HB(register_pair);\
		++emu->i8080.PC;\
		return 5;\
	}

INX_INSTR(b,B,C)
INX_INSTR(d,D,E)
INX_INSTR(h,H,L)

INSTR(inx_sp) {
	++emu->i8080.SP;
	++emu->i8080.PC;

	return 5;
}

// Decrement Word Instructions

// Macro for defining dcx instructions */
#define DCX_INSTR(r,R,R1)\
	INSTR(dcx_##r) {\
		uint16_t register_pair = ((emu->i8080.R << 8) | emu->i8080.R1) - 1;\
		emu->i8080.R1 = LB(register_pair);\
		emu->i8080.R  = HB(register_pair);\
		++emu->i8080.PC;\
		return 5;\
	}

DCX_INSTR(b,B,C)
DCX_INSTR(d,D,E)
DCX_INSTR(h,H,L)

INSTR(dcx_sp) {
	--emu->i8080.SP;
	++emu->i8080.PC;

	return 5;
}

// Double Byte Add Instructions

// Macro for defining dad instructions
#define DAD_INSTR(r,R,R1)\
	INSTR(dad_##r)	{\
		uint16_t HL = (emu->i8080.H << 8) | emu->i8080.L;\
		uint16_t register_pair = (emu->i8080.R << 8) | emu->i8080.R1;\
		i8080emu_set_flag(emu, FLAG_C, (HL + register_pair) > 65535);\
		HL += register_pair;\
		emu->i8080.L = LB(HL);\
		emu->i8080.H = HB(HL);\
		++emu->i8080.PC;\
		return 10;\
	}

DAD_INSTR(b,B,C)
DAD_INSTR(d,D,E)
DAD_INSTR(h,H,L)

INSTR(dad_sp) {
	// HL += SP;
	uint16_t HL = (emu->i8080.H << 8) | emu->i8080.L;	// Put together the values held by H and L.
	i8080emu_set_flag(emu, FLAG_C, (HL + emu->i8080.SP) > 65535);// Carry Flag.

	HL += emu->i8080.SP;
	
	// Put each byte in their own register.
	emu->i8080.L = LB(HL);
	emu->i8080.H = HB(HL);

	++emu->i8080.PC;

	return 10;
}

// 16 Bit Transfer Instructions

// Macro for defining lxi instructions
#define LXI_INSTR(r,R,R1)\
	INSTR(lxi_##r) {\
		emu->i8080.R1 = i8080emu_read_byte_memory(emu, emu->i8080.PC + 1);\
		emu->i8080.R = i8080emu_read_byte_memory(emu, emu->i8080.PC + 2);\
		emu->i8080.PC += 3;\
		return 10;\
	}
		
LXI_INSTR(b,B,C)
LXI_INSTR(d,D,E)
LXI_INSTR(h,H,L)

INSTR(lxi_sp) {
	// Loads word in SP.
	emu->i8080.SP = get_word_from_instruction(emu);

	emu->i8080.PC += 3;

	return 10;
}

INSTR(shld) {
	// Stores HL at addr, addr +1.
	// First, put address into a variable.
	uint16_t addr = get_word_from_instruction(emu);

	// Places HL in memory.
	i8080emu_write_word_memory(emu, addr, (emu->i8080.H << 8) | emu->i8080.L);

	emu->i8080.PC += 3;

	return 16;
}

INSTR(lhld) {
	// Loads a word from addr, addr+1 to HL
	// First, put address into a variable.
	uint16_t addr = get_word_from_instruction(emu);

	// Loads the value from memory.
	emu->i8080.L = i8080emu_read_byte_memory(emu, addr);
	emu->i8080.H = i8080emu_read_byte_memory(emu, addr + 1);

	emu->i8080.PC += 3;

	return 16;
}

INSTR(xthl) {
	// Swap HL with SP, SP+1
	uint8_t H = emu->i8080.H;
	uint8_t L = emu->i8080.L;

	emu->i8080.H = i8080emu_read_byte_memory(emu, emu->i8080.SP + 1);
	emu->i8080.L = i8080emu_read_byte_memory(emu, emu->i8080.SP);

	i8080emu_write_word_memory(emu, emu->i8080.SP, (H << 8) | L);

	++emu->i8080.PC;

	return 18;
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

	return 5;
}

INSTR(sphl) {
	// SP = HL.
	emu->i8080.SP = (emu->i8080.H << 8) | emu->i8080.L;

	++emu->i8080.PC;

	return 5;
}
