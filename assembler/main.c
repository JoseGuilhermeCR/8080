#include <stdio.h>

#include "error.h"
#include "preprocessor.h"
#include "file_buffer.h"

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
		return 1;
	
	struct fbuffer *buffer = fbuffer_from(argv[1]);

	if (!buffer)
		return 1;

	if (preprocess(buffer))
		printf("\033[38;2;0;255;0mPreprocessor Finished!\033[0m\n");
	else
		printf("\033[38;2;255;0;0mPreprocessor Failed! Stopping...\033[0m\n");
		
	printf("PREPROCESSED:\n%s\n", buffer->data);
//	assemble(fbuffer);

	fbuffer_destroy(buffer);

	return 0;
}
