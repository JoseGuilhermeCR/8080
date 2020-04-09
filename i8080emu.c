/* José Guilherme de C. Rodrigues - 03/2020 */
#include "i8080emu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
	i8080emu *i = i8080emu_create();
	i8080emu_load_program_into_memory(i, "invaders/invaders");
	
	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_print_registers(i);
	i8080emu_cycle(i);
	i8080emu_print_registers(i);

	i8080emu_destroy(i);

	return 0;
}

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

// NOPS will be substituted by a NULL.
const void (*instruction_table[0x100]) (i8080emu *emu) = {
	NULL, lxi_b, stax_b, inx_b, inr_b, dcr_b, mvi_b, rlc,
	NULL, dad_b, ldax_b, dcx_b, inr_c, dcr_c, mvi_c, rrc,
	NULL, lxi_d, stax_d, inx_d, inr_d, dcr_d, mvi_d, ral,
	NULL, dad_d, ldax_d, dcx_d, inr_e, dcr_e, mvi_e, rar,
	NULL, lxi_h, shld, inx_h, inr_h, dcr_h, mvi_h, NULL, 		/* RIM is NULL. TODO:SecondNULL DAA*/
	NULL, dad_h, lhld, dcx_h, inr_l, dcr_l, mvi_l, cma,
	NULL, lxi_sp, sta, inx_sp, inr_m, dcr_m, mvi_m, stc, 		/*SIM is NULL*/
	NULL, dad_sp, lda, dcx_sp, inr_a, dcr_a, mvi_a, cmc,
	NULL, mov_bc, mov_bd, mov_be, mov_bh, mov_bl, mov_bm, mov_ba,	/* Not including MOV R,R (where R is any Register). */
	mov_cb, NULL, mov_cd, mov_ce, mov_ch, mov_cl, mov_cm, mov_ca,
	mov_db, mov_dc, NULL, mov_de, mov_dh, mov_dl, mov_dm, mov_da,
	mov_eb, mov_ec, mov_ed, NULL, mov_eh, mov_el, mov_em, mov_ea,
	mov_hb, mov_hc, mov_hd, mov_he, NULL, mov_hl, mov_hm, mov_ha,
	mov_lb, mov_lc, mov_ld, mov_le, mov_lh, NULL, mov_lm, mov_la,
	mov_mb, mov_mc, mov_md, mov_me, mov_mh, mov_ml, hlt, mov_ma,
	mov_ab, mov_ac, mov_ad, mov_ae, mov_ah, mov_al, mov_am, NULL,
	add_b, add_c, add_d, add_e, add_h, add_l, add_m, add_a,
	adc_b, adc_c, adc_d, adc_e, adc_h, adc_l, adc_m, adc_a,
	sub_b, sub_c, sub_d, sub_e, sub_h, sub_l, sub_m, sub_a,
	sbb_b, sbb_c, sbb_d, sbb_e, sbb_h, sbb_l, sbb_m, sbb_a,
	ana_b, ana_c, ana_d, ana_e, ana_h, ana_l, ana_m, ana_a,
	xra_b, xra_c, xra_d, xra_e, xra_h, xra_l, xra_m, xra_a,
	ora_b, ora_c, ora_d, ora_e, ora_h, ora_l, ora_m, ora_a,
	cmp_b, cmp_c, cmp_d, cmp_e, cmp_h, cmp_l, cmp_m, cmp_a,
	rnz, pop_b, jnz, jmp, cnz, push_b, adi, rst_0,
	rz, ret, jz, NULL, cz, call, aci, rst_1,
	rnc, pop_d, jnc, NULL /* OUT */, cnc, push_d, sui, rst_2,
	rc, NULL, jc, NULL /* IN */, cc, NULL, sbi, rst_3,
	rpo, pop_h, jpo, xthl, cpo, push_h, ani, rst_4,
	rpe, pchl, jpe, xchg, cpe, NULL, xri, rst_5,
	rp, pop_psw, jp, NULL /* DI */, cp, push_psw, ori, rst_6,
	rm, sphl, jm, NULL /* EI */, cm, NULL, cpi, rst_7
};

/* Setup functions */
i8080emu *i8080emu_create() {
	i8080emu *emu = malloc(sizeof(i8080emu));

	// Set all registers to 0.
	emu->i8080.A = 0x00;
	emu->i8080.F = 0x00;
	emu->i8080.B = 0x00;
	emu->i8080.C = 0x00;
	emu->i8080.D = 0x00;
	emu->i8080.E = 0x00;
	emu->i8080.H = 0x00;
	emu->i8080.L = 0x00;
	emu->i8080.PC  = 0x0000; //0x0000;
	emu->i8080.SP  = 0x0000;

	// Allocate memory, for now 65536 bytes (max possible).
	emu->memory = malloc(sizeof(uint8_t) * 0x10000);
	memset(emu->memory, 0x00, 0x10000);

	return emu;
}

/* We may want to choose where in memory the program will be loaded. */
void i8080emu_load_program_into_memory(i8080emu *emu, const char *filename) {
	FILE *file = fopen(filename, "rb");

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	fread(emu->memory, size, 1, file);

	fclose(file);
}

void i8080emu_destroy(i8080emu *emu) {
	free(emu->memory);
	free(emu);
	emu = NULL;
}

/* Emulation functions */
bool i8080_get_flag(const i8080 *i8080, Flags flag) {
	return i8080->F & flag;
}

void i8080_set_flag(i8080 *i8080, Flags flag, bool value) {
	if (value)
		i8080->F |= flag;
	else
		i8080->F &= ~flag;
}

void i8080emu_cycle(i8080emu *emu) {
	if (instruction_table[emu->memory[emu->i8080.PC]]) {
		(*instruction_table[emu->memory[emu->i8080.PC]])(emu);
	} else {
		puts("\nInstruction was null.\n");
		++emu->i8080.PC;
	}
}

/* Help */
uint8_t get_byte_hl(const i8080emu *emu) {
	return emu->memory[(emu->i8080.H << 8) | emu->i8080.L];
}

uint16_t get_word_from_instruction(i8080emu *emu) {
	return (emu->memory[emu->i8080.PC + 2] << 8) | emu->memory[emu->i8080.PC + 1];
}

uint8_t get_byte_from_instruction(i8080emu *emu) {
	return emu->memory[emu->i8080.PC + 1];
}

/* Debug Stuff */
void i8080emu_print_registers(i8080emu *emu) {
	printf(
		"A: %02x F: %02x\n"
		"B: %02x C: %02x\n"
		"D: %02x E: %02x\n"
		"H: %02x L: %02x\n"
		"SP: %04x\n"
		"PC: %04x\n",
		emu->i8080.A,  emu->i8080.F,
		emu->i8080.B,  emu->i8080.C,
		emu->i8080.D,  emu->i8080.E,
		emu->i8080.H,  emu->i8080.L,
		emu->i8080.SP,
		emu->i8080.PC
	);
}
