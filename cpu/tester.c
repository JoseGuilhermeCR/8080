#include "i8080emu.h"
#include "../disassembler/disassembler.h"
#include <stdio.h>
#include <stdlib.h>

void make_test(const char *filename);

int main() {
	make_test("tests/8080PRE.COM");

	return 0;
}

void make_test(const char *filename) {
	i8080emu *emu = i8080emu_create();
	i8080emu_load_program_into_memory(emu, filename, 0x100);

	// Put a return instruction where the test will make a call.
	emu->memory[0x0005] = 0xc9;

	emu->i8080.PC = 0x0100;
	while (true) {
		puts("Will execute now: ");
		// Disassembles next instruction and print on screen.
		disassemble_8080(emu->memory, emu->i8080.PC);
		// Executes one instruction (all instructions take more than 1 cycle).
		i8080emu_run_cycles(emu, 1);
		
		puts("Registers after execution:");
		i8080emu_print_registers(emu);

		if (emu->i8080.PC == 0x0005) {
			printf("Called 0005\n");
			printf("Value in C: %u\n", emu->i8080.C);

			if (emu->i8080.C == 9) {
				uint16_t addr = (emu->i8080.D << 8) | emu->i8080.E;
				while (emu->memory[addr] != '$') {
					putchar(emu->memory[addr]);
					++addr;
				}
			} else if (emu->i8080.C == 5) {
				printf("%c", emu->i8080.C);
			}

			emu->i8080.PC = 0x0100;
		}

		if (emu->i8080.PC == 0) {
			puts("Error occurred!");
			exit(1);
		}

		puts("");
	}

	i8080emu_destroy(emu);
}
