extern "C" {
	#include "cpu/i8080emu.h"
	#ifdef WITH_DISASSEMBLER
	#include "disassembler/disassembler.h"
	#endif
}

#include <iostream>

void make_test(const std::string &filename, bool step_by_step);

int main() {
	make_test("cpu/tests/TEST.COM", false);
	make_test("cpu/tests/8080PRE.COM", false);
	make_test("cpu/tests/8080EXM.COM", false);
	make_test("cpu/tests/CPUTEST.COM", true);

	return 0;
}

void make_test(const std::string &filename, bool step_by_step) {
	std::cout << "\n=====================================================\n";

	i8080emu *emu = i8080emu_create();
	i8080emu_load_program_into_memory(emu, filename.c_str(), 0x100, true);

	// Put a return instruction where the test will make a call.
	i8080emu_write_byte_memory(emu, 0x0005, 0xC9);

	long done_cycles = 0;
	long done_instructions = 0;

	uint16_t pc_before;
	emu->i8080.PC = 0x0100;
	while (true/*!term.closed()*/) {
		#ifdef WITH_DISASSEMBLER
		if (step_by_step) {
			std::cout << "\nPress enter to execute: ";
		}
		disassemble_8080(emu->memory, emu->i8080.PC);
		if (step_by_step) {
			std::cin.ignore();
		}
		#endif	

		pc_before = emu->i8080.PC;
		done_cycles += i8080emu_run_cycles(emu, 1);
		++done_instructions;

		#ifdef WITH_DISASSEMBLER
		std::cout << "Registers after execution:\n";
		i8080emu_print_registers(emu);
		#endif

		if (emu->i8080.PC == 0x0005) {
			if (emu->i8080.C == 9) {
				uint16_t addr = (emu->i8080.D << 8) | emu->i8080.E;

				uint8_t byte;
				while ((byte = i8080emu_read_byte_memory(emu, addr)) != '$') {
					std::cout << byte;
					++addr;
				}
			} else if (emu->i8080.C == 2) {
				std::cout << emu->i8080.E;
			}
		}


		if (emu->i8080.PC == 0) {
			std::cout << "\nGot to 0000, PC before = " << std::hex << std::uppercase << pc_before << "H " << std::dec << pc_before;
			std::cout << "\nDone instructions: " << done_instructions << ". Done cycles: " << done_cycles;
			break;
		}
	}

	i8080emu_destroy(emu);

	std::cout << "\n=====================================================\n";
}
