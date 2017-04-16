#include "unit.h"
#include "../arg/parse.h"
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef UNIT_NO_COLOR
#define COLOR_BLUE "\x1B[34m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_RED "\x1B[31m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_END "\x1B[0m"
#else
#define COLOR_BLUE
#define COLOR_GREEN
#define COLOR_RED
#define COLOR_YELLOW
#define COLOR_END
#endif

#define COLORIZE(color, s) color s COLOR_END

#define JMP_PASS 1
#define JMP_FAIL 2
#define JMP_SKIP 3

typedef unit_FnSuite FnSuite;
typedef unit_FnTest FnTest;
typedef unit_G Global;
typedef unit_S Suite;
typedef unit_T Test;
typedef unit_Trace Trace;

typedef enum unit_Mode {
    UNIT_MODE_TEST,
    UNIT_MODE_HELP,
} Mode;

struct unit_G {
    struct {
        bool is_verbose;
        Mode mode;
        const arg_Option* options;
        arg_ParseResult parseResult;
    } config;
    struct {
        size_t passed;
        size_t failed;
        size_t skipped;
    } stats;
    struct {
        const char* suite;
        const char* test;
    } target;
};

struct unit_S {
    const Global* g;
    const char* name;
    struct {
        size_t passed;
        size_t failed;
        size_t skipped;
    } stats;
};

struct unit_T {
    const Suite* s;
    const char* name;
    jmp_buf jmp;
};

static void initGlobal(Global* g, const int argc, const char** argv);
static void modeTest(Global* g);
static void modeHelp(const char* binaryName, const Global* g);

int main(const int argc, const char* argv[])
{
    Global g;
    initGlobal(&g, argc, argv);

    if (!g.config.parseResult.ok || g.config.parseResult.tailc != 0) {
        fprintf(stderr, "Unexpected argument: %s.\n"
                        "Try %s --help for more information.\n",
            g.config.parseResult.tailv[0], argv[0]);

        exit(EXIT_FAILURE);
    }

    switch (g.config.mode) {
    case UNIT_MODE_TEST:
        modeTest(&g);
        break;

    case UNIT_MODE_HELP:
        modeHelp(argv[0], &g);
        break;
    }
    exit(g.stats.failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

void initGlobal(Global* g, const int argc, const char** argv)
{
    assert(g != NULL);
    assert(argv != NULL);

    memset(g, 0, sizeof(Global));

    static const arg_Option options[]
        = { { 'h', "help", "Print this help message and exit.", NULL },
            { 's', "suite", "Run only suite with name <SUITE>.", "SUITE" },
            { 't', "test", "Run only test with name <TEST>.", "TEST" },
            { 'v', "verbose", "Print more testing details.", NULL }, { 0 } };
    const char* out[4] = { 0 };

    const arg_ParseResult result = arg_parse(argc - 1, &argv[1], options, out);

    g->config.is_verbose = out[3] != NULL;
    g->config.mode = out[0] != NULL ? UNIT_MODE_HELP : UNIT_MODE_TEST;
    g->config.options = options;
    g->config.parseResult = result;

    g->target.suite = out[1];
    g->target.test = out[2];
}

void modeTest(Global* g)
{
    unit_main(g);

    printf(
        COLORIZE(COLOR_BLUE, "TOTAL") " Passed " COLORIZE(COLOR_GREEN,
            "%zu") ","
                   " failed " COLORIZE(COLOR_RED,
                       "%zu") ","
                              " skipped " COLORIZE(COLOR_YELLOW, "%zu") ".\n",
        g->stats.passed, g->stats.failed, g->stats.skipped);
}

void modeHelp(const char* binaryName, const Global* g)
{
    printf(
        "Usage: %s [options...] [--] [arguments...]\nOptions:\n", binaryName);
    arg_fprintOptions(stdout, g->config.options);
}

void unit_namedSuite(Global* g, const char* name, FnSuite suite)
{
    assert(g != NULL && "Global state pointer is required.");
    assert(name != NULL && "Suite name is required.");
    assert(suite != NULL && "Suite function is required.");

    if (g->target.suite != NULL && strcmp(g->target.suite, name) != 0) {
        return;
    }

    if (g->config.is_verbose) {
        printf(COLORIZE(COLOR_BLUE, "SUITE") " %s\n", name);
    }

    Suite s = {.g = g, .name = name };
    suite(&s);

    if (g->config.is_verbose) {
        printf(COLORIZE(COLOR_BLUE, "RESULT") " %s " COLORIZE(
                   COLOR_BLUE, "::") " Passed " COLORIZE(COLOR_GREEN,
                   "%zu") ","
                          " failed " COLORIZE(COLOR_RED,
                              "%zu") ","
                                     " skipped " COLORIZE(
                                         COLOR_YELLOW, "%zu") ".\n\n",
            name, s.stats.passed, s.stats.failed, s.stats.skipped);
    }

    g->stats.passed += s.stats.passed;
    g->stats.failed += s.stats.failed;
    g->stats.skipped += s.stats.skipped;
}

void unit_namedTest(Suite* s, const char* name, FnTest test)
{
    assert(s != NULL && "Suite state pointer is required.");
    assert(name != NULL && "Test name is required.");
    assert(test != NULL && "Test function is required.");

    if (s->g->target.test != NULL && strcmp(s->g->target.test, name) != 0) {
        return;
    }

    unit_T t = {.s = s, .name = name };
    switch (setjmp(t.jmp)) {
    case 0:
        test(&t);

    case JMP_PASS:
        if (s->g->config.is_verbose) {
            printf(COLORIZE(COLOR_GREEN, "PASS") " %s.%s\n", t.s->name, t.name);
        }
        s->stats.passed += 1;
        break;

    case JMP_FAIL:
        s->stats.failed += 1;
        break;

    case JMP_SKIP:
        s->stats.skipped += 1;
        break;

    default:
        assert(false);
    }
}

void unit_pass(unit_T* t)
{
    assert(t != NULL);

    longjmp(t->jmp, JMP_PASS);
}

void unit_failtf(unit_T* t, const Trace trace, const char* format, ...)
{
    assert(t != NULL);
    assert(format != NULL);

    printf(COLORIZE(COLOR_RED, "FAIL") " %s.%s (%s:%d)\n    ", t->s->name,
        t->name, trace.file, trace.line);

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    puts("\n");

    longjmp(t->jmp, JMP_FAIL);
}

void unit_skiptf(unit_T* t, const Trace trace, const char* format, ...)
{
    assert(t != NULL);
    assert(format != NULL);

    if (t->s->g->config.is_verbose) {
        printf(COLORIZE(COLOR_YELLOW, "SKIP") " %s.%s (%s:%d)\n    ",
            t->s->name, t->name, trace.file, trace.line);

        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);

        puts("\n");
    }

    longjmp(t->jmp, JMP_SKIP);
}
