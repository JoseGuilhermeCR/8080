#include <stdio.h>

#include "preprocessor.h"
#include "file_buffer.h"
#include "assembler.h"
#include "error.h"

/* My assembly:
 * Lines starting with a . must be a valid assembler command.
 * .org, .dw, .db, .ascii, .asciiz
 * Lines starting with a # must be a valid preprocessor command.
 * #include #define
 * The rest of lines are program lines.
 * They may have the following pattern
 * [label]: mnemonic register arithmetic_value
 * arithmetic_value can also be a solvable expression during assembly.
 * */

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		ERROR("[8080_ASSEMBLER] You need to specify at least one file.\n");
		return 1;
	}

	struct fbuffer *buffer = fbuffer_from(argv[1]);
	if (!buffer)
		return 1;

	preprocess(buffer);
	fbuffer_optimize(buffer);	
	printf("%s\n", buffer->data);
	struct assembler_out out = assemble(buffer);

	assembler_destroy_out(&out);
	fbuffer_destroy(buffer);

	return 0;
}
