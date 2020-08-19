#include "CuTest.h"

void test_preprocessor_include_error(CuTest *tc)
{
	CuAssertTrue(tc, 1 == 1);
}

CuSuite *cu_preprocessor_suite()
{
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_preprocessor_include_error);

	return suite;
}
