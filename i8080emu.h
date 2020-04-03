/* José Guilherme de C. Rodrigues - 03/2020 */
#ifndef __I8080EMU_H__
#define __I8080EMU_H__

#include <stdint.h>
#include <stdbool.h>

#define HB(x) (x & 0xff00) >> 8 // HIGH_BYTE
#define LB(x) (x & 0x00ff)	// LOW_BYTE 

/* Bits 4, 3 and 1 (starting from 0) don't have use. */
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

extern uint8_t parity_table[0x100];
extern void (*instruction_table[0x100]) (i8080emu *emu);

/* Setup Stuff. */
i8080emu *i8080emu_create();
void i8080emu_load_program_into_memory(i8080emu *emu, const char *filename);
void i8080emu_destroy(i8080emu *emu);

/* Emulation Stuff. */
bool i8080_get_flag(const i8080 *i8080, Flags flag);
void i8080_set_flag(i8080 *i8080, Flags flag, bool value);

void i8080emu_cycle(i8080emu *emu);

/* Instructions */
/* Macro for declaring an instruction */
#define INSTR(x) void x(i8080emu *emu)

/* 8 Bit Transfer Instructions */
INSTR(mvi_b);
INSTR(mvi_c);
INSTR(stax_b);
INSTR(stax_d);
INSTR(ldax_b);

/* 16 Bit Transfer Instructions */
INSTR(lxi_b);
INSTR(lxi_d);

/* Increment Byte Instructions */
INSTR(inr_b);
INSTR(inr_c);

/* Decrement Byte Instructions */
INSTR(dcr_b);
INSTR(dcr_c);

/* Increment Register Pair Instructions */
INSTR(inx_b);

/* Decrement Register Pair Instructions */
INSTR(dcx_b);

/* Rotate Instructions */
INSTR(rlc);
INSTR(rrc);

/* Double Byte Add Instructions */
INSTR(dad_b);

/* Help */
bool carry_out_lower_nibble(uint8_t a, uint8_t b);
bool even_parity(uint8_t a);

/* Debug Stuff. */
void i8080emu_print_registers(i8080emu  *emu);

#endif
