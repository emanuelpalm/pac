#include "parse.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

typedef arg_Option Option;
typedef arg_ParseResult ParseResult;

typedef struct arg_Context Context;

typedef bool (*FnOptionMatcher)(const char* arg, const Option* opt);

struct arg_Context {
    int argc;
    const char** argv;
    const Option* opts;
    const char** out;
};

typedef enum arg_Kind {
    ARGV_KIND_SHORT,
    ARGV_KIND_LONG,
    ARGV_KIND_STOP,
    ARGV_KIND_VALUE,
    ARGV_KIND_NULL,
} Kind;

static ParseResult parse(Context*);
static Kind kind(const char*);
static ParseResult parseOption(FnOptionMatcher, Context*);
static ParseResult takeOption(const size_t, const Option*, Context*);
static bool optionIsPair(const Option*);
static bool matchShort(const char*, const Option*);
static bool matchLong(const char*, const Option*);

void arg_fprintOption(FILE* stream, const Option* opt)
{
    assert(stream != NULL);
    assert(opt != NULL);
    assert(opt->name != NULL);
    assert(isalnum(opt->key) || opt->key == ' ');

    fprintf(stream, " %c%c", (opt->key != ' ' ? '-' : ' '), opt->key);

    const char* name = opt->name;
    char buffer[32];
    if (opt->valueType != NULL) {
        snprintf(buffer, sizeof(buffer), "%s <%s>", name, opt->valueType);
        name = buffer;
    }
    fprintf(stream, " --%-20s %s\r\n", name, opt->description);
}

void arg_fprintOptions(FILE* stream, const Option opts[])
{
    assert(stream != NULL);
    assert(opts != NULL);

    const Option* opt = opts;
    for (; opt->name != NULL; opt = &opt[1]) {
        arg_fprintOption(stream, opt);
    }
}

ParseResult arg_parse(
    int argc, const char** argv, const Option opts[], const char** out)
{
    assert(opts != NULL);
    assert(argv != NULL);
    assert(out != NULL);

    return parse(&(Context){ argc, argv, opts, out });
}

ParseResult parse(Context* ctx)
{
    assert(ctx != NULL);

    switch (kind(ctx->argv[0])) {
    case ARGV_KIND_SHORT:
        return parseOption(matchShort, ctx);

    case ARGV_KIND_LONG:
        return parseOption(matchLong, ctx);

    case ARGV_KIND_STOP:
        ctx->argc -= 1;
        ctx->argv = &ctx->argv[1];

    default:
        return (
            ParseResult){.tailc = ctx->argc, .tailv = ctx->argv, .ok = true };
    }
}

Kind kind(const char* arg)
{
    if (arg == NULL) {
        return ARGV_KIND_NULL;
    }
    if (arg[0] == '-') {
        switch (arg[1]) {
        case '-':
            return arg[2] != '\0' ? ARGV_KIND_LONG : ARGV_KIND_STOP;

        case '\0':
            break;

        default:
            return ARGV_KIND_SHORT;
        }
    }
    return ARGV_KIND_VALUE;
}

ParseResult parseOption(FnOptionMatcher matches, Context* ctx)
{
    assert(matches != NULL);

    const char* arg = ctx->argv[0];
    const Option* opt = ctx->opts;
    for (size_t index = 0; opt->name != NULL; opt = &opt[1], ++index) {
        if (matches(arg, opt)) {
            return takeOption(index, opt, ctx);
        }
    }
    return (ParseResult){.tailc = ctx->argc, .tailv = ctx->argv, .ok = false };
}

ParseResult takeOption(const size_t index, const Option* opt, Context* ctx)
{
    assert(opt != NULL);
    assert(opt->name != NULL);
    assert(ctx != NULL);

    size_t offset;
    if (optionIsPair(opt) && kind(ctx->argv[1]) == ARGV_KIND_VALUE) {
        offset = 2;
        ctx->out[index] = ctx->argv[1];
    } else {
        offset = 1;
        ctx->out[index] = "";
    }
    ctx->argc -= offset;
    ctx->argv = &(ctx->argv)[offset];

    return parse(ctx);
}

bool optionIsPair(const Option* opt)
{
    assert(opt != NULL);

    return opt->valueType != NULL;
}

bool matchShort(const char* arg, const Option* opt)
{
    assert(arg != NULL);
    assert(opt != NULL);

    return opt->key != ' ' && arg[1] == opt->key && arg[2] == '\0';
}

bool matchLong(const char* arg, const Option* opt)
{
    assert(arg != NULL);
    assert(opt != NULL);

    return strcmp(&arg[2], opt->name) == 0;
}
