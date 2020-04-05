/* José Guilherme de C. Rodrigues - 04/2020 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Just generates the parity table used in the emulator. */
int main(int argc, char **argv) {
	unsigned max = 256;
	unsigned bits = 8;

	printf("Parity of numbers from %u to %u\n", 0, max - 1);
	// For each number.
	for (unsigned i = 0; i < max; ++i) {
		// Calculate its parity (count the number of 1 bits).
		unsigned parity = 0;
		for (unsigned u = 0; u < bits; ++u) {
			if (i & (unsigned)pow(2, u))
				++parity;
		}

		if (i % 16 == 0)
			printf("\n");

		printf("%u, ", (parity % 2 == 0) ? 1 : 0);
	}
}
