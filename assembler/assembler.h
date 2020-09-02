#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <stdbool.h>

#include "file_buffer.h"
#include "vecu8.h"

struct assembler_out {
	bool success;
	struct vecu8 *machine_code;
};

struct assembler_out assemble(struct fbuffer *buffer);
void assembler_destroy_out(struct assembler_out *out);

#endif
