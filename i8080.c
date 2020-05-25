#include "cpu/i8080emu.h"
#include "disassembler/disassembler.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	uint16_t start_pc;
	uint16_t memory_offset;
	bool debug;
	bool step_by_step;
} run_parameters;

void run(const char *filename, const run_parameters *parameters);
void make_test(const char *filename, const run_parameters *parameters);
void interrupt_test();
void step(i8080emu *const emu);

int main(int argc, char **argv) {
	if (argc <= 1) {
		printf(
			"How to execute:\n"
			"i8080 file.hex (extras: --start_pc --memory_offset --debug --step_by_step)\n"
			"i8080 cpu_tests (extras: --debug --step_by_step(depends on debug))\n"
			"i8080 interrupt_test\n"
		);
		return 1;
	} else {
		run_parameters parameters;
		parameters.start_pc = 0x0100;
		parameters.memory_offset = 0x0100;

		for (int i = 2; i < argc; ++i) {
			if (strncmp(argv[i], "--debug", 7) == 0) {
				parameters.debug = true;
			} else if (strncmp(argv[i], "--step_by_step", 14) == 0) {
				parameters.step_by_step = true;
			} else if (strncmp(argv[i], "--start_pc", 10) == 0) {
				if (i + 1 < argc && argv[i + 1][0] != '-') {
					parameters.start_pc = atoi(argv[i + 1]);
				}
			} else if (strncmp(argv[i], "--memory_offset", 15) == 0) {
				if (i + 1 < argc && argv[i + 1][0] != '-') {
					parameters.memory_offset = atoi(argv[i + 1]);
				}
			}
		}

		printf("Starting with: start_pc = 0x%02X memory_offset = 0x%02X debug = %i step_by_step = %i\n", parameters.start_pc, parameters.memory_offset, parameters.debug,
														 parameters.step_by_step);
	
		if (strncmp(argv[1], "cpu_tests", 9) == 0) {
			make_test("cpu/tests/TEST.COM", &parameters);
			make_test("cpu/tests/8080PRE.COM", &parameters);
			make_test("cpu/tests/8080EXM.COM", &parameters);
			make_test("cpu/tests/CPUTEST.COM", &parameters);
		} else if (strncmp(argv[1], "interrupt_test", 14) == 0) {
			interrupt_test();
		} else {
			run(argv[1], &parameters);
		}
	}
	
	return 0;
}

void run(const char *filename, const run_parameters *parameters) {
	i8080emu *emu = i8080emu_create();

	if (!i8080emu_load_program_into_memory(emu, filename, parameters->memory_offset, parameters->debug)) {
		i8080emu_destroy(emu);
		return;
	}

	unsigned long done_cycles = 0;
	unsigned long done_instructions = 0;

	emu->i8080.PC = parameters->start_pc;

	while (true) {
		if (parameters->debug) {
			if (parameters->step_by_step) {
				printf("\nPress enter to execute: ");
			}
			disassemble_8080(emu->memory, emu->i8080.PC);
			if (parameters->step_by_step) {
				getchar();
			}
		}

		done_cycles += i8080emu_step(emu);
		if (!emu->is_halted) {
			++done_instructions;
		}

		if (parameters->debug) {
			printf("Registers after execution:\n");
			i8080emu_print_debug_info(emu);
			printf("Executed cycles: %lu Instructions executed: %lu\n", done_cycles, done_instructions);
		}
	}

	i8080emu_destroy(emu);
}

void make_test(const char *filename, const run_parameters *parameters) {
	printf("\n=====================================================\n");

	i8080emu *emu = i8080emu_create();
	i8080emu_load_program_into_memory(emu, filename, parameters->memory_offset, parameters->debug);

	// Put a return instruction where the test will make a call.
	i8080emu_write_byte_memory(emu, 0x0005, 0xC9);

	unsigned long done_cycles = 0;
	unsigned long done_instructions = 0;

	uint16_t pc_before;
	emu->i8080.PC = parameters->start_pc;
	while (true) {
		if (parameters->debug) {
			if (parameters->step_by_step) {
				printf("\nPress enter to execute: ");
			}
			disassemble_8080(emu->memory, emu->i8080.PC);
			if (parameters->step_by_step) {
				getchar();
			}
		}

		pc_before = emu->i8080.PC;
		done_cycles += i8080emu_step(emu);
		++done_instructions;

		if (parameters->debug) {
			printf("Registers after execution:\n");
			i8080emu_print_debug_info(emu);
		}

		if (emu->i8080.PC == 0x0005) {
			if (emu->i8080.C == 9) {
				uint16_t addr = (emu->i8080.D << 8) | emu->i8080.E;

				uint8_t byte;
				while ((byte = i8080emu_read_byte_memory(emu, addr)) != '$') {
					printf("%c", byte);
					++addr;
				}
			} else if (emu->i8080.C == 2) {
				printf("%c", emu->i8080.E);
			}
		}


		if (emu->i8080.PC == 0) {
			printf("\nGot to 0000, PC before = 0x%04X.", pc_before);
			printf("\nDone instructions: %lu. Done cyles: %lu.", done_instructions, done_cycles);
			break;
		}
	}

	i8080emu_destroy(emu);

	printf("\n=====================================================\n");
}

void interrupt_test() {
	i8080emu *emu = i8080emu_create();
	emu->i8080.PC = 0x0100; // Program starts at 0x0100.

	// Writing by hand the program.
	i8080emu_write_byte_memory(emu, 0x0100, 0x31);	// LXI SP,
	i8080emu_write_word_memory(emu, 0x0101, 0x0FF7);// LXI SP, 0FF7
	i8080emu_write_byte_memory(emu, 0x0103, 0x3C);	// INR A
	i8080emu_write_byte_memory(emu, 0x0104, 0x3D);	// DCR A
	i8080emu_write_byte_memory(emu, 0x0105, 0x76);	// HLT
	i8080emu_write_byte_memory(emu, 0x0106, 0x00);	// NOP

	// Writing by hand the subroutine located at 0x0000. This should be called by RST 0.
	i8080emu_write_byte_memory(emu, 0x0000, 0xF5);	// PUSH PSW.
	i8080emu_write_byte_memory(emu, 0x0001, 0x16);	// MVI D,
	i8080emu_write_byte_memory(emu, 0x0002, 0xFF);	// MVI D, FF
	i8080emu_write_byte_memory(emu, 0x0003, 0x7A);	// MOV A, D
	i8080emu_write_byte_memory(emu, 0x0004, 0xBA);	// CMP D
	i8080emu_write_byte_memory(emu, 0x0005, 0xFB);	// EI
	i8080emu_write_byte_memory(emu, 0x0006, 0xF1);	// POP PSW
	i8080emu_write_byte_memory(emu, 0x0007, 0xC9);	// RET

	// Start of Test
	printf("Initial state of machine: \n");
	i8080emu_print_debug_info(emu);

	step(emu);			// LXI SP, 0FF7
	step(emu);			// INR A

	// Start of Interrupt
	printf("\nJust sent an interrupt! 0xC7 sent to CPU.\n");
	i8080emu_interrupt(emu, 0xC7);	// Interrupts, and tells the CPU to execute RST 0.

	step(emu);			// CPU was interrupted, this should be RST 0.

	step(emu);			// PUSH PSW

	// Trying to send an interrupt while the system is disabled.
	printf("\nJust sent an interrupt! 0xC7 sent to CPU (Should not get caught! System is disabled!)\n");
	i8080emu_interrupt(emu, 0xC7);	// This interrupt won't be caught, because the interrupt system is disabled at this point!

	step(emu);			// MVI D, FF
	step(emu);			// MOV A, D
	step(emu);			// CMP D
	step(emu);			// EI
	step(emu);			// POP PSW
	step(emu);			// RET
	// End of Interrupt

	step(emu);			// DCR A
	step(emu);			// HLT
	step(emu);			// MACHINE HALTED, NOTHING HAPPES UNTIL AN INTERRUPT OCCURS.
	step(emu);			// MACHINE HALTED, NOTHING HAPPES UNTIL AN INTERRUPT OCCURS.

	// Start of Interrupt
	printf("\nJust sent an interrupt! 0xC7 sent to CPU.\n");
	i8080emu_interrupt(emu, 0xC7);
	step(emu);			// Since an interrupt happened, execution would continue.
	step(emu);			// PUSH PSW
	step(emu);			// MVI D, FF
	step(emu);			// MOV A, D
	step(emu);			// CMP D
	step(emu);			// EI
	step(emu);			// POP PSW
	step(emu);			// RET
	// End of Interrupt
	
	step(emu);			// NOP
	// End of Test

	i8080emu_destroy(emu);
}

void step(i8080emu *const emu) {
	printf("\nPress enter to execute: ");
	if (emu->was_interrupted && emu->i8080.INTE == 0x1) {
		printf("0x%02X (Instruction from interrupt)\n", emu->inte_instruction);
	} else {
		if (emu->is_halted) {
			printf("Machine is halted, nothing will be done until an interrupt occurs.\n");
		} else {
			disassemble_8080(emu->memory, emu->i8080.PC);
		}
	}
	getchar();

	i8080emu_step(emu);
	printf("Registers after execution:\n");
	i8080emu_print_debug_info(emu);
}
