#include "CuTest.h"
#include "../string_manipulation.h"

void test_extract_from_quotes_valid(CuTest *tc)
{
	const char *line = "label: .ascizz \"Hello, There!\"";
	const size_t size = 30;

	char *quote = extract_from_quotes(line, size);

	CuAssertPtrNotNull(tc, quote);
	CuAssertStrEquals(tc, "Hello, There!", quote);

	free(quote);
}

void test_extract_from_quotes_empty_quotes_valid(CuTest *tc)
{
	const char *line = "label: .ascizz \"\"";
	const size_t size = 17;

	char *quote = extract_from_quotes(line, size);

	CuAssertPtrNotNull(tc, quote);
	CuAssertStrEquals(tc, "" , quote);

	free(quote);
}

void test_extract_from_quotes_null_fail(CuTest *tc)
{
	const char *fail_line = "";
	const size_t fail_size = 0;

	char *fail_quote = extract_from_quotes(fail_line, fail_size);

	CuAssertTrue(tc, fail_quote == NULL);
}

void test_extract_from_quotes_missing_quotes_fail(CuTest *tc)
{
	const char *fail_line = "label: .asciiz Hello, There!";
	const size_t fail_size = 28;

	char *fail_quote = extract_from_quotes(fail_line, fail_size);

	CuAssertTrue(tc, fail_quote == NULL);
}

void test_extract_from_quotes_missing_one_fail(CuTest *tc)
{
	const char *fail_line = "label: .asciiz \"Hello, There!";
	const size_t fail_size = 29;

	char *fail_quote = extract_from_quotes(fail_line, fail_size);

	CuAssertTrue(tc, fail_quote == NULL);
}

CuSuite *cu_string_manipulation_suite()
{
	CuSuite *suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, test_extract_from_quotes_valid);
	SUITE_ADD_TEST(suite, test_extract_from_quotes_empty_quotes_valid);
	SUITE_ADD_TEST(suite, test_extract_from_quotes_null_fail);
	SUITE_ADD_TEST(suite, test_extract_from_quotes_missing_quotes_fail);
	SUITE_ADD_TEST(suite, test_extract_from_quotes_missing_one_fail);

	return suite;
}
