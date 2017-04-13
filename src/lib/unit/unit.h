#ifndef LIB_UNIT_UNIT_H
#define LIB_UNIT_UNIT_H

/// Unit testing framework.
///
/// The framework divides individual unit tests into groups referred to as
/// suites. All suites and unit tests are defined as regular C functions.
/// Suites declare their tests by calling unit_test(), and the special
/// unit_main() function declares all suites by calling unit_suite().
///
/// By convention, each suite should be declared in a separate file, with its
/// unit tests being inside the same file. Suite files should have file names
/// ending with `.unit.c`. The main file, conventionally named `main.unit.c`,
/// simply needs to contain one function prototype for each suite, as well as
/// declaring the unit_main() function.
///
/// ## Example
///
/// This simple example shows two suites, each declaring two unit tests.
///
/// ### Code
///
/// ```c
/// // main.unit.c
/// #include "unit.h"
///
/// void math(unit_S *s);
/// void text(unit_S *s);
///
/// void unit_main(unit_G *g, const int argc, const char *argv[]) {
///     unit_suite(g, math);
///     unit_suite(g, text);
/// }
///
/// // math.unit.c
/// #include "unit.h"
///
/// void shouldAdd(unit_T *t) {
///     int x = 10 + 1;
///     UNIT_ASSERT_EQI(t, 11, x); // Passes.
/// }
///
/// void shouldSub(unit_T *t) {
///     UNIT_ASSERT(t, (10 - 1) == 99); // Fails.
/// }
///
/// void math(unit_S *s) {
///     unit_test(s, shouldAdd);
///     unit_test(s, shouldSub);
/// }
///
/// // text.unit.c
/// #include "unit.h"
///
/// void shouldUppercase(unit_T *t) {
///     unit_pass(t); // Passes.
/// }
///
/// void shouldLowercase(unit_T *t) {
///     unit_skipf(t, "Cannot lowercase."); // Skipped.
/// }
///
/// void text(unit_S *s) {
///     unit_test(s, shouldUppercase);
///     unit_test(s, shouldLowercase);
/// }
/// ```
///
/// ### Output
///
/// Running the example generates the below output.
///
/// ```sh
/// $ ./tests
/// FAIL math.shouldSub (/path/to/math.unit.c:9)
///     (10 - 1) == 99
///
/// TOTAL Passed 2, failed 1, skipped 1.
/// ```
///
/// If wanting to configure the output of the test executable, the
/// `./tests --help` command may be used from a terminal to get a list of
/// available options.
///
/// \file
///
/// \see unit_main()

#include "../meta/attribute.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/// Asserts that given expression is true.
///
/// If assertion fails, the currently executed test is failed and the assert
/// statement is printed.
///
/// \param t          Pointer to test context.
/// \param expression Expression to evaluate.
///
/// \see unit_failf()
#define UNIT_ASSERT(t, expression) UNIT_ASSERTF(t, expression, #expression)

/// Asserts that given expression is true.
///
/// If assertion fails, the currently executed test is failed and the given
/// format string, with any
/// provided arguments, is printed.
///
/// \param t          Pointer to test context.
/// \param expression Expression to evaluate.
/// \param format     String format.
/// \param ...        Format arguments.
///
/// \see unit_failf()
#define UNIT_ASSERTF(t, expression, ...)           \
    if (!(expression)) {                           \
        unit_failtf(t, UNIT_TRACE(), __VA_ARGS__); \
    }

/// Asserts that given characters are equal.
///
/// If assertion fails, the currently executed test is failed and the compared
/// characters are
/// printed.
///
/// \param t Pointer to test context.
/// \param a First character comparand.
/// \param b Second character comparand.
#define UNIT_ASSERT_EQC(t, a, b)                                  \
    do {                                                          \
        const char a0 = (char)(a);                                \
        const char b0 = (char)(b);                                \
        if (a0 != b0) {                                           \
            unit_failtf(t, UNIT_TRACE(), "'%c' != '%c'", a0, b0); \
        }                                                         \
    } while (0)

/// Asserts that given floating point numbers are equal, given provided
/// precision.
///
/// If assertion fails, the currently executed test is failed and the compared
/// numbers are printed.
///
/// \param t         Pointer to test context.
/// \param a         First double comparand.
/// \param b         Second double comparand.
/// \param precision Precision of double comparison (e.g. 0.005).
#define UNIT_ASSERT_EQF(t, a, b, precision)                       \
    do {                                                          \
        const double a0 = (double)(a);                            \
        const double b0 = (double)(b);                            \
        if (fabs(a0 - b0) > (precision)) {                        \
            unit_failtf(t, UNIT_TRACE(), "'%f' != '%f'", a0, b0); \
        }                                                         \
    } while (0)

/// Asserts that given signed integers are equal.
///
/// If assertion fails, the currently executed test is failed and the compared
/// integers are
/// printed.
///
/// \param t Pointer to test context.
/// \param a First integer comparand.
/// \param b Second integer comparand.
#define UNIT_ASSERT_EQI(t, a, b)                                    \
    do {                                                            \
        const intmax_t a0 = (intmax_t)(a);                          \
        const intmax_t b0 = (intmax_t)(b);                          \
        if (a0 != b0) {                                             \
            unit_failtf(t, UNIT_TRACE(), "'%ji' != '%ji'", a0, b0); \
        }                                                           \
    } while (0)

/// Asserts that given pointers are equal.
///
/// If assertion fails, the currently executed test is failed and the addresses
/// of the compared
/// pointers are printed.
///
/// \param t Pointer to test context.
/// \param a First pointer comparand.
/// \param b Second pointer comparand.
#define UNIT_ASSERT_EQP(t, a, b)                                           \
    do {                                                                   \
        const void *a0 = (void *)(a);                                      \
        const void *b0 = (void *)(b);                                      \
        if (a0 != b0) {                                                    \
            unit_failtf(t, UNIT_TRACE(), #a " %#p != " #b " %#p", a0, b0); \
        }                                                                  \
    } while (0)

/// Asserts that given zero-terminated C strings are equal.
///
/// If assertion fails, the currently executed test is failed and the compared
/// strings are printed.
///
/// \param t Pointer to test context.
/// \param a First C string comparand.
/// \param b Second C string comparand.
#define UNIT_ASSERT_EQS(t, a, b)                                              \
    do {                                                                      \
        const char *a0 = (a);                                                 \
        const char *b0 = (b);                                                 \
        if (a0 == NULL || b0 == NULL || strcmp(a0, b0) != 0) {                \
            unit_failtf(t, UNIT_TRACE(), "\"%*.s\" != \"%*.s\"", length0, a0, \
                        length0, b0);                                         \
        }                                                                     \
    } while (0)

/// Asserts that given unsigned integers are equal.
///
/// If assertion fails, the currently executed test is failed and the compared
/// integers are printed.
///
/// \param t Pointer to test context.
/// \param a First unsigned integer comparand.
/// \param b Second unsigned integer comparand.
#define UNIT_ASSERT_EQU(t, a, b)                                    \
    do {                                                            \
        const uintmax_t a0 = (uintmax_t)(a);                        \
        const uintmax_t b0 = (uintmax_t)(b);                        \
        if (a0 != b0) {                                             \
            unit_failtf(t, UNIT_TRACE(), "'%ju' != '%ju'", a0, b0); \
        }                                                           \
    } while (0)

/// Unit call trace macro.
///
/// This macro expands into a call trace which describes the file and line at
/// which the macro is placed.
///
/// The macro is intended to be use together with the unit_pass(), unit_failf()
/// and unit_skipf() functions in order for these to be able to properly print
/// the contexts in which they are called.
///
/// \returns Call trace.
///
/// \see unit_Trace
#define UNIT_TRACE() \
    (unit_Trace) { __FILE__, (int)(__LINE__) }

/// Global state context.
///
/// \see unit_main()
typedef struct unit_G unit_G;

/// Suite state context.
///
/// \see unit_suite()
typedef struct unit_S unit_S;

/// Test state context.
///
/// \see unit_test()
typedef struct unit_T unit_T;

/// Test suite function type.
///
/// \see unit_suite()
typedef void (*unit_FnSuite)(unit_S *s);

/// Unit test function type.
///
/// \see unit_test()
typedef void (*unit_FnTest)(unit_T *t);

/// Unit call trace.
///
/// Represents the file and line at which a pass, fail or skip call is made. The
/// type is typically instantiated using the UNIT_TRACE() macro.
///
/// \see UNIT_TRACE()
typedef struct unit_Trace {
    /// Name of file in which trace was generated.
    const char *file;

    /// File line at which trace was generated.
    int line;
} unit_Trace;

/// Unit testing main function.
///
/// The implementation is to be provided by the user of this framework. This as
/// the framework itself takes ownership of the regular main() function, and
/// calls this function from there.
///
/// The function is responsible for registering test suites. The provided
/// command line arguments do not include options first consumed by the
/// framework.
///
/// \param g Pointer to global state context.
///
/// \see unit_suite()
void unit_main(unit_G *g);

/// Declares test suite with same name as given function and executes
/// associated tests.
///
/// \param g     Pointer to global state context.
/// \param suite Pointer to suite function.
///
/// \see unit_namedSuite()
#define unit_suite(g, suite) unit_namedSuite(g, #suite, suite)

/// Declares named test suite and executes associated tests.
///
/// \param g     Pointer to global state context.
/// \param name  Suite name.
/// \param suite Pointer to suite function.
///
/// \see unit_suite()
void unit_namedSuite(unit_G *g, const char *name, unit_FnSuite suite);

/// Declares unit test with same name as given function and executes it.
///
/// \param s    Pointer to suite state context.
/// \param test Pointer to test function.
///
/// \see unit_namedTest()
#define unit_test(s, test) unit_namedTest(s, #test, test)

/// Declares unit test with same name as given function and executes it.
///
/// \param s     Pointer to suite state context.
/// \param name  Test name.
/// \param suite Pointer to test function.
///
/// \see unit_test()
void unit_namedTest(unit_S *s, const char *name, unit_FnTest test);

/// Passes test `t`.
///
/// \param t Pointer to unit test context.
void unit_pass(unit_T *t) ATTRIBUTE_NORETURN();

/// Fails current test, reporting given formatted message.
///
/// \param t      Pointer to unit test context.
/// \param format String format.
/// \param ...    Format arguments.
///
/// \see unit_failtf()
#define unit_failf(t, ...) unit_failtf(t, UNIT_TRACE(), __VA_ARGS__)

/// Fails test `t` with provided trace and formatted message.
///
/// \param t      Pointer to unit test context.
/// \param trace  Call trace.
/// \param format String format.
/// \param ...    Format arguments.
///
/// \see unit_failf()
void unit_failtf(unit_T *t, unit_Trace trace, const char *format, ...)
    ATTRIBUTE_NORETURN() ATTRIBUTE_FORMAT_PRINTF(3, 4);

/// Skips current test, reporting given formatted message.
///
/// \param t      Pointer to unit test context.
/// \param format String format.
/// \param ...    Format arguments.
///
/// \see unit_skiptf()
#define unit_skipf(t, ...) unit_skiptf(t, UNIT_TRACE(), __VA_ARGS__)

/// Skips test `t` with provided trace and formatted message.
///
/// \param t      Pointer to unit test context.
/// \param trace  Call trace.
/// \param format String format.
/// \param ...    Format arguments.
///
/// \see unit_skipf()
void unit_skiptf(unit_T *t, unit_Trace trace, const char *format, ...)
    ATTRIBUTE_NORETURN() ATTRIBUTE_FORMAT_PRINTF(3, 4);

#endif
