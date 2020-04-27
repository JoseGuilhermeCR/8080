#include "cpu/i8080emu.h"
#include "disassembler/disassembler.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void make_test(const char *filename, bool debug, bool step_by_step);
void interrupt_test();
void step(i8080emu *const emu);

int main(int argc, char **argv) {
	if (argc <= 1) {
		printf(
			"How to execute:\n"
			"tester cputests (if you want to debug or debug step by step, please change the function call for the test you want)\n"
			"tester interrupts\n"
		);
		return 1;
	} else {
		if (strncmp(argv[1], "cputests", 8) == 0) {
			make_test("cpu/tests/TEST.COM", false, false);
			make_test("cpu/tests/8080PRE.COM", false, false);
			make_test("cpu/tests/8080EXM.COM", false, false);
			make_test("cpu/tests/CPUTEST.COM", false, false);
		} else if (strncmp(argv[1], "interrupts", 10) == 0) {
			interrupt_test();
		} else {
			printf("Unrecognized parameter\n");
			return 1;
		}
	}
	
	return 0;
}

void make_test(const char *filename, bool debug, bool step_by_step) {
	printf("\n=====================================================\n");

	i8080emu *emu = i8080emu_create();
	i8080emu_load_program_into_memory(emu, filename, 0x100, true);

	// Put a return instruction where the test will make a call.
	i8080emu_write_byte_memory(emu, 0x0005, 0xC9);

	unsigned long done_cycles = 0;
	unsigned long done_instructions = 0;

	uint16_t pc_before;
	emu->i8080.PC = 0x0100;
	while (true) {
		if (debug) {
			if (step_by_step) {
				printf("\nPress enter to execute: ");
			}
			disassemble_8080(emu->memory, emu->i8080.PC);
			if (step_by_step) {
				getchar();
			}
		}

		pc_before = emu->i8080.PC;
		done_cycles += i8080emu_step(emu);
		++done_instructions;

		if (debug) {
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
