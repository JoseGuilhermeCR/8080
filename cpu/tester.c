#include "i8080emu.h"

#ifdef WITH_DISASSEMBLER
#include "../disassembler/disassembler.h"
#endif

#include <stdio.h>
#include <stdlib.h>

void make_test(const char *filename);

int main() {
	//make_test("tests/CPUTEST.COM");
	make_test("tests/8080PRE.COM");
	make_test("tests/8080EXER.COM");

	return 0;
}

void make_test(const char *filename) {
	printf("\n=====================================================\n");
	bool success = false;

	i8080emu *emu = i8080emu_create();
	i8080emu_load_program_into_memory(emu, filename, 0x100, true);

	// Put a return instruction where the test will make a call.
	i8080emu_write_byte_memory(emu, 0x0005, 0xC9);

	emu->i8080.PC = 0x0100;
	while (true) {
		#ifdef WITH_DISASSEMBLER
		printf("Will execute now:\n");
		disassemble_8080(emu->memory, emu->i8080.PC);
		#endif

		i8080emu_run_cycles(emu, 1);

		#ifdef WITH_DISASSEMBLER
//		printf("Registers after execution:\n");
//		i8080emu_print_registers(emu);
		#endif

		if (emu->i8080.PC == 0x0005) {
			if (emu->i8080.C == 9) {
				uint16_t addr = (emu->i8080.D << 8) | emu->i8080.E;

				uint8_t byte;
				while ((byte = i8080emu_read_byte_memory(emu, addr)) != '$') {
					putchar(byte);
					++addr;
				}

				success = true;
			} else if (emu->i8080.C == 2) {
				putchar((char)emu->i8080.E);
			}

		}

		if (emu->i8080.PC == 0) {
			if (!success)
				printf("An error occurred!\n");
			break;
		}
	}

	i8080emu_destroy(emu);

	printf("\n=====================================================\n");
}
