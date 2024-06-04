/** Common functions for tests. */

#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

/**
 * Returns whether two double values are nearly equal,
 * i.e. within 10 ** -7 of each other.
 * Floating-point math is approximate, so isclose() is preferable to ==.
 * There are some exceptions: ints (<= 53 bits) and fractions whose denominators
 * are powers of 2 (e.g. 0.5 or 0.75) can be represented exactly as a double.
 */
bool isclose(double d1, double d2);

/**
 * Return if the components of two vectors are exactly equal.
 * Floating-point math is approximate, so the result of a
 * series of vector computations might not be exactly as expected.
 */
bool vec_equal(vector_t v1, vector_t v2);

/**
 * Return if two vectors are close to each other; that is, if the corresponding
 * components are within 1e-7 of each other.
 * This may be more useful than vec_equal, because vector components are
 * doubles, not integers, and floating-point math is approximate.
 */
bool vec_isclose(vector_t v1, vector_t v2);

/**
 * Returns whether two double values are nearly equal,
 * where the acceptable difference is specified by epsilon.
 */
bool within(double epsilon, double d1, double d2);

/**
 * Returns whether two vectors are nearly equal,
 * where the acceptable difference of each component is specified by epsilon.
 */
bool vec_within(double epsilon, vector_t v1, vector_t v2);

/**
 * Open the file 'filename', read one word into 'testname', and close the file.
 * If the file cannot be found, exit with error.
 */
void read_testname(char *filename, char *testname, size_t testname_size);

/*
 * This macro checks whether to run the test function (which will be true
 * if the test is called without command-line arguments).
 * Then, it calls the test function, and prints "<test-func> PASS".
 *
 * The macro can simply be called as
 *      DO_TEST(test_func_name)
 * It doesn't matter if 'test_func_name' is a valid variable, because
 * the macro is a simple text replacement done *before* compilation.
 * No ; is needed after the closing ), because again, the macro is a simple
 * text replacement, and the statements already have ;s.
 */
#define DO_TEST(TEST_FN)                                                       \
  if (all_tests || strcmp(testname, #TEST_FN) == 0) {                          \
    TEST_FN();                                                                 \
    puts(#TEST_FN " PASS");                                                    \
  }

/**
 * Executes function 'run' and returns whether it causes an assertion failure,
 * as detected by a SIGABRT signal.
 * Passes the auxiliary argument 'aux' to 'run', i.e. calls run(aux).
 */
bool test_assert_fail(void (*run)(void *aux), void *aux);

#endif // #ifndef __TEST_UTIL_H__
