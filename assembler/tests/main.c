#include <stdio.h>

#include "CuTest.h"

extern CuSuite *cu_preprocessor_suite();
extern CuSuite *cu_string_manipulation_suite();

int main()
{
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, cu_preprocessor_suite());
	CuSuiteAddSuite(suite, cu_string_manipulation_suite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	return 0;
}
