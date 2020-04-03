/* José Guilherme de C. Rodrigues - 03/2020 */
#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include <stdint.h>

// To print OPs that are followed by a byte or a word.
#define PRINT_OP_BYTE(x) printf("%s$%02x", x, code[1])
#define PRINT_OP_WORD(x) printf("%s$%02x%02x", x, code[2], code[1])

// Table with number of bytes of each OP.
extern uint8_t op_bytes[0x100];
// Table with each op.
extern const char *op_lookup[0x100];

uint8_t disassemble_8080(const uint8_t *buffer, uint32_t pc);

#endif
