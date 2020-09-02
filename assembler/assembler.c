#include "assembler.h"
#include "error.h"

struct assembler_out assemble(struct fbuffer *buffer)
{
	struct assembler_out out = { true, vecu8_create(0) };
	
	if (!out.machine_code)
		goto error;


	return out;
error:
	out.success = false;
	return out;
}

void assembler_destroy_out(struct assembler_out *out)
{
	if (out->machine_code)
		vecu8_destroy(out->machine_code);
}
