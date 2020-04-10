/* José Guilherme de C. Rodrigues - 03/2020 */
#include "disassembler.h"
#include <stdio.h>
#include <stdlib.h>

/* 8080 works with little endian for WORDS (2 byte values).
 * For example, "LXI B,word" will load a word into registers BC.
 * The first byte goes into C and the second into B, because it's 
 * in little endian. */

uint8_t instruction_bytes[0x100] = {
	1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
	1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
	1, 3, 3, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 2, 1,
	1, 3, 3, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 2, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 1, 3, 3, 2, 1,
	1, 1, 3, 2, 3, 1, 2, 1, 1, 1, 3, 2, 3, 1, 2, 1,
	1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 1, 2, 1,
	1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 1, 2, 1
};

const char *instruction_lookup[0x100] = {
	"NOP", "LXI\tB,#", "STAX\tB", "INX\tB", "INR\tB", "DCR\tB", "MVI\tB,#", "RLC",
	"NOP", "DAD\tB", "LDAX\tB", "DCX\tB", "INR\tC", "DCR\tC", "MVI\tC,#", "RRC",
	"NOP", "LXI\tD,#", "STAX\tD", "INX\tD", "INR\tD", "DCR\tD", "MVI\tD,#", "RAL",
	"NOP", "DAD\tD", "LDAX\tD", "DCX\tD", "INR\tE", "DCR\tE", "MVI\tE,#", "RAR",
	"RIM", "LXI\tH,#", "SHLD\t", "INX\tH", "INR\tH", "DCR\tH", "MVI\tH,#", "DAA",
	"NOP", "DAD\tH", "LHLD\t", "DCX\tH", "INR\tL", "DCR\tL", "MVI\tL,#", "CMA",
	"SIM", "LXI\tSP,#", "STA\t", "INX\tSP", "INR\tSP", "DCR\tM", "MVI\tM,#", "STC",
	"NOP", "DAD\tSP", "LDA\t", "DCX\tSP", "INR\tA", "DCR\tA", "MVI\tA,#", "CMC",
	"MOV\tB,B", "MOV\tB,C", "MOV\tB,D", "MOV\tB,E", "MOV\tB,H", "MOV\tB,L", "MOV\tB,M", "MOV\tB,A",
	"MOV\tC,B", "MOV\tC,C", "MOV\tC,D", "MOV\tC,E", "MOV\tC,H", "MOV\tC,L", "MOV\tC,M", "MOV\tC,A",
	"MOV\tD,B", "MOV\tD,C", "MOV\tD,D", "MOV\tD,E", "MOV\tD,H", "MOV\tD,L", "MOV\tD,M", "MOV\tD,A",
	"MOV\tE,B", "MOV\tE,C", "MOV\tE,D", "MOV\tE,E", "MOV\tE,H", "MOV\tE,L", "MOV\tE,M", "MOV\tE,A",
	"MOV\tH,B", "MOV\tH,C", "MOV\tH,D", "MOV\tH,E", "MOV\tH,H", "MOV\tH,L", "MOV\tH,M", "MOV\tH,A",
	"MOV\tL,B", "MOV\tL,C", "MOV\tL,D", "MOV\tL,E", "MOV\tL,H", "MOV\tL,L", "MOV\tL,M", "MOV\tL,A",
	"MOV\tM,B", "MOV\tM,C", "MOV\tM,D", "MOV\tM,E", "MOV\tM,H", "MOV\tM,L", "HLT", "MOV\tM,A",
	"MOV\tA,B", "MOV\tA,C", "MOV\tA,D", "MOV\tA,E", "MOV\tA,H", "MOV\tA,L", "MOV\tA,M", "MOV\tA,A",
	"ADD\tB", "ADD\tC", "ADD\tD", "ADD\tE", "ADD\tH", "ADD\tL", "ADD\tM", "ADD\tA",
	"ADC\tB", "ADC\tC", "ADC\tD", "ADC\tE", "ADC\tH", "ADC\tL", "ADC\tM", "ADC\tA",
	"SUB\tB", "SUB\tC", "SUB\tD", "SUB\tE", "SUB\tH", "SUB\tL", "SUB\tM", "SUB\tA",
	"SBB\tB", "SBB\tC", "SBB\tD", "SBB\tE", "SBB\tH", "SBB\tL", "SBB\tM", "SBB\tA",
	"ANA\tB", "ANA\tC", "ANA\tD", "ANA\tE", "ANA\tH", "ANA\tL", "ANA\tM", "ANA\tA",
	"XRA\tB", "XRA\tC", "XRA\tD", "XRA\tE", "XRA\tH", "XRA\tL", "XRA\tM", "XRA\tA",
	"ORA\tB", "ORA\tC", "ORA\tD", "ORA\tE", "ORA\tH", "ORA\tL", "ORA\tM", "ORA\tA",
	"CMP\tB", "CMP\tC", "CMP\tD", "CMP\tE", "CMP\tH", "CMP\tL", "CMP\tM", "CMP\tA",
	"RNZ", "POP\tB", "JNZ\t", "JMP\t", "CNZ\t", "PUSH\tB", "ADI\t#", "RST\t0",
	"RZ", "RET", "JZ\t", "NOP", "CZ\t", "CALL\t", "ACI\t#", "RST\t1",
	"RNC", "POP\tD", "JNC\t", "OUT\t#", "CNC\t", "PUSH\tD", "SUI\t#", "RST\t2",
	"RC", "NOP", "JC\t", "IN\t#", "CC\t", "NOP", "SBI\t#", "RST\t3",
	"RPO", "POP\tH", "JPO\t", "XTHL", "CPO\t", "PUSH\tH", "ANI\t#", "RST\t4",
	"RPE", "PCHL", "JPE\t", "XCHG", "CPE\t", "NOP", "XRI\t#", "RST\t5",
	"RP", "POP\tPSW", "JP\t", "DI", "CP\t", "PUSH\tPSW", "ORI\t#", "RST\t6",
	"RM", "SPHL", "JM\t", "EI\t", "CM\t", "NOT", "CPI\t#", "RST\t7"
};

/* This function may misinterpret data with actual instructions, but it should work for most 
 * part of the programs. */
uint8_t disassemble_8080(const uint8_t *buffer, uint32_t pc) {
	// A pointer to the location of pc in buffer.
	const uint8_t *code = &buffer[pc];

	// Get number of bytes the instruction has.
	uint8_t bytes = instruction_bytes[*code];

	// Print pc position.
	printf("[%04x]\t", pc);
	// Print instruction and it's data if necessary.
	if (bytes == 1) {
		printf("%s", instruction_lookup[*code]);
	} else if (bytes == 2) {
		PRINT_OP_BYTE(instruction_lookup[*code]);
	} else if (bytes == 3) {
		PRINT_OP_WORD(instruction_lookup[*code]);
	}
	printf("\n");

	return bytes;
}

#ifndef WITH_DISASSEMBLER
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Error: Please provide the file to be disassembled.\n");
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");
	
	if (file != NULL) {
		// Get size of the file.
		fseek(file, 0, SEEK_END);
		long fsize = ftell(file);
		fseek(file, 0, SEEK_SET);	

		// Read file to memory.
		uint8_t *buffer = malloc(sizeof(char) * fsize);
		fread(buffer, fsize, 1, file);
		fclose(file);

		uint32_t pc = 0;
		while (pc < fsize) {
			pc += disassemble_8080(buffer, pc);
		}
	} else {
		printf("Error Couldn't open %s\n", argv[1]);
		return 1;
	}

	return 0;
}
#endif
