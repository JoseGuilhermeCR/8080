/* José Guilherme de C. Rodrigues - 03/2020 */
#ifndef __I8080EMU_H__
#define __I8080EMU_H__

#include <stdint.h>
#include <stdbool.h>

#define HB(x) (x & 0xff00) >> 8 // HIGH_BYTE
#define LB(x) (x & 0x00ff)	// LOW_BYTE 
#define LN(x) (x & 0xf)		// LOW_NIBBLE

#define RESET_FLAG 0x02

/* Bits 4, 3 and 1 (starting from 0) don't have use. And
 * have default values. The Flag Register should aways be:
 * |S,Z,0,A,0,P,1,C| */
typedef enum {
	FLAG_S = 0x1 << 7,	// Sign Flag.
	FLAG_Z = 0x1 << 6,	// Zero Flag.
	FLAG_A = 0x1 << 4,	// Auxiliary Carry Flag.
	FLAG_P = 0x1 << 2,	// Parity Flag.
	FLAG_C = 0x1		// Carry Flag.
} Flags;

typedef struct {
	/* Registers */
	uint8_t A;	// AF is a register pair (PSW program status word).
	uint8_t F;
	uint8_t B;	// BC is a register pair.
	uint8_t C;
	uint8_t D;	// DE is a register pair.
	uint8_t E;
	uint8_t H;	// HL is a register pair.
	uint8_t L;
	uint16_t SP;	// Stack Pointer.
	uint16_t PC;	// Program Counter.
} i8080;

typedef struct {
	i8080 i8080;
	uint8_t *memory; // The maximum size of memory is 65536 bytes.
} i8080emu;

extern const uint8_t parity_table[0x100];
extern const uint8_t (*instruction_table[0x100]) (i8080emu *emu);

/* Setup Stuff. */
i8080emu *i8080emu_create();
void i8080emu_load_program_into_memory(i8080emu *emu, const char *filename, uint16_t offset, bool print_info);
void i8080emu_destroy(i8080emu *emu);

/* Emulation Stuff. */
bool i8080_get_flag(const i8080 *i8080, Flags flag);
void i8080_set_flag(i8080 *i8080, Flags flag, bool value);

unsigned i8080emu_run_cycles(i8080emu *emu, unsigned cycles);

/* Instructions */
/* Macro for declaring an instruction */
#define INSTR(x) uint8_t x(i8080emu *emu)

/* 8 Bit Transfer Instructions */
INSTR(mvi_b);
INSTR(mvi_c);
INSTR(mvi_d);
INSTR(mvi_e);
INSTR(mvi_h);
INSTR(mvi_l);
INSTR(mvi_m);
INSTR(mvi_a);
INSTR(stax_b);
INSTR(stax_d);
INSTR(sta);
INSTR(ldax_b);
INSTR(ldax_d);
INSTR(lda);

INSTR(mov_bb);
INSTR(mov_bc);
INSTR(mov_bd);
INSTR(mov_be);
INSTR(mov_bh);
INSTR(mov_bl);
INSTR(mov_bm);
INSTR(mov_ba);

INSTR(mov_cb);
INSTR(mov_cc);
INSTR(mov_cd);
INSTR(mov_ce);
INSTR(mov_ch);
INSTR(mov_cl);
INSTR(mov_cm);
INSTR(mov_ca);

INSTR(mov_db);
INSTR(mov_dc);
INSTR(mov_dd);
INSTR(mov_de);
INSTR(mov_dh);
INSTR(mov_dl);
INSTR(mov_dm);
INSTR(mov_da);

INSTR(mov_eb);
INSTR(mov_ec);
INSTR(mov_ed);
INSTR(mov_ee);
INSTR(mov_eh);
INSTR(mov_el);
INSTR(mov_em);
INSTR(mov_ea);

INSTR(mov_hb);
INSTR(mov_hc);
INSTR(mov_hd);
INSTR(mov_he);
INSTR(mov_hh);
INSTR(mov_hl);
INSTR(mov_hm);
INSTR(mov_ha);

INSTR(mov_lb);
INSTR(mov_lc);
INSTR(mov_ld);
INSTR(mov_le);
INSTR(mov_lh);
INSTR(mov_ll);
INSTR(mov_lm);
INSTR(mov_la);

INSTR(mov_mb);
INSTR(mov_mc);
INSTR(mov_md);
INSTR(mov_me);
INSTR(mov_mh);
INSTR(mov_ml);
INSTR(mov_ma);

INSTR(mov_ab);
INSTR(mov_ac);
INSTR(mov_ad);
INSTR(mov_ae);
INSTR(mov_ah);
INSTR(mov_al);
INSTR(mov_am);
INSTR(mov_aa);

/* 16 Bit Transfer Instructions */
INSTR(lxi_b);
INSTR(lxi_d);
INSTR(lxi_h);
INSTR(lxi_sp);
INSTR(shld);
INSTR(lhld);
INSTR(xthl);
INSTR(xchg);
INSTR(sphl);

/* Increment Byte Instructions */
INSTR(inr_b);
INSTR(inr_c);
INSTR(inr_d);
INSTR(inr_e);
INSTR(inr_h);
INSTR(inr_l);
INSTR(inr_m);
INSTR(inr_a);

/* Decrement Byte Instructions */
INSTR(dcr_b);
INSTR(dcr_c);
INSTR(dcr_d);
INSTR(dcr_e);
INSTR(dcr_h);
INSTR(dcr_l);
INSTR(dcr_m);
INSTR(dcr_a);

/* Add Byte Instructions */
INSTR(add_b);
INSTR(add_c);
INSTR(add_d);
INSTR(add_e);
INSTR(add_h);
INSTR(add_l);
INSTR(add_m);
INSTR(add_a);
INSTR(adi);

/* Add Byte with Carry In Instructions */
INSTR(adc_b);
INSTR(adc_c);
INSTR(adc_d);
INSTR(adc_e);
INSTR(adc_h);
INSTR(adc_l);
INSTR(adc_m);
INSTR(adc_a);
INSTR(aci);

/* Sub Byte Instructions */
INSTR(sub_b);
INSTR(sub_c);
INSTR(sub_d);
INSTR(sub_e);
INSTR(sub_h);
INSTR(sub_l);
INSTR(sub_m);
INSTR(sub_a);
INSTR(sui);

/* Sub Byte With Borrow-In Instructions */
INSTR(sbb_b);
INSTR(sbb_c);
INSTR(sbb_d);
INSTR(sbb_e);
INSTR(sbb_h);
INSTR(sbb_l);
INSTR(sbb_m);
INSTR(sbb_a);
INSTR(sbi);

/* Increment Word Instructions */
INSTR(inx_b);
INSTR(inx_d);
INSTR(inx_h);
INSTR(inx_sp);

/* Decrement Word Instructions */
INSTR(dcx_b);
INSTR(dcx_d);
INSTR(dcx_h);
INSTR(dcx_sp);

/* Rotate Instructions */
INSTR(rlc);
INSTR(rrc);
INSTR(ral);
INSTR(rar);

/* Logical And Instructions */
INSTR(ana_b);
INSTR(ana_c);
INSTR(ana_d);
INSTR(ana_e);
INSTR(ana_h);
INSTR(ana_l);
INSTR(ana_m);
INSTR(ana_a);
INSTR(ani);

/* Logical Xor Instructions */
INSTR(xra_b);
INSTR(xra_c);
INSTR(xra_d);
INSTR(xra_e);
INSTR(xra_h);
INSTR(xra_l);
INSTR(xra_m);
INSTR(xra_a);
INSTR(xri);

/* Logical Or Instructions */
INSTR(ora_b);
INSTR(ora_c);
INSTR(ora_d);
INSTR(ora_e);
INSTR(ora_h);
INSTR(ora_l);
INSTR(ora_m);
INSTR(ora_a);
INSTR(ori);

/* Logical Compare Instructions */
INSTR(cmp_b);
INSTR(cmp_c);
INSTR(cmp_d);
INSTR(cmp_e);
INSTR(cmp_h);
INSTR(cmp_l);
INSTR(cmp_m);
INSTR(cmp_a);
INSTR(cpi);

/* Double Byte Add Instructions */
INSTR(dad_b);
INSTR(dad_d);
INSTR(dad_h);
INSTR(dad_sp);

/* Accumulator and Flag Instructions */
INSTR(cma);
INSTR(stc);
INSTR(cmc);

/* Branch Control Instructions */
INSTR(rnz);
INSTR(rz);
INSTR(ret);
INSTR(rnc);
INSTR(rc);
INSTR(rpo);
INSTR(rpe);
INSTR(rp);
INSTR(rm);

INSTR(jnz);
INSTR(jz);
INSTR(jmp);
INSTR(jnc);
INSTR(jc);
INSTR(jpo);
INSTR(jpe);
INSTR(jp);
INSTR(jm);

INSTR(cnz);
INSTR(cz);
INSTR(call);
INSTR(cnc);
INSTR(cc);
INSTR(cpo);
INSTR(cpe);
INSTR(cp);
INSTR(cm);

INSTR(rst_0);
INSTR(rst_1);
INSTR(rst_2);
INSTR(rst_3);
INSTR(rst_4);
INSTR(rst_5);
INSTR(rst_6);
INSTR(rst_7);

INSTR(pchl);

/* Stack Operation Instructions */
INSTR(pop_b);
INSTR(push_b);
INSTR(pop_d);
INSTR(push_d);
INSTR(pop_h);
INSTR(push_h);
INSTR(pop_psw);
INSTR(push_psw);

/* Constrol Instructions */
INSTR(nop);
INSTR(hlt);

/* Help */
uint8_t get_byte_hl(const i8080emu *emu);
uint16_t get_word_from_instruction(const i8080emu *emu);
uint8_t get_byte_from_instruction(const i8080emu *emu);

/* Memory manipulation */
void i8080emu_write_byte_memory(i8080emu *emu, uint16_t addr, uint8_t byte);
void i8080emu_write_word_memory(i8080emu *emu, uint16_t addr, uint16_t word);
uint8_t i8080emu_read_byte_memory(const i8080emu *emu, uint16_t addr);
uint16_t i8080emu_read_word_memory(const i8080emu *emu, uint16_t addr);
/* Debug Stuff. */
void i8080emu_print_registers(i8080emu  *emu);

#endif
