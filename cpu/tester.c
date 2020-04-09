#include "i8080emu.h"
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
		printf("About to execute instruction: %02X\n", emu->memory[emu->i8080.PC]);
		i8080emu_run_cycles(emu, 1); // Will never execute more than one instruction.
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
			printf("\n Error occurred!");
			exit(1);
		}
	}

	i8080emu_destroy(emu);
}
