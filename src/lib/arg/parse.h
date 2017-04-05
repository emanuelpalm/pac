#ifndef LIBRARY_ARG_PARSE_H
#define LIBRARY_ARG_PARSE_H

/**
 * Argument vector parsing utilities.
 *
 * This file provides data types and functions relevant especially when parsing
 * the `argc`/`argv` arguments provided to the application `main()` function.
 *
 * @file
 *
 * @see arg_parse()
 */

#include <stdbool.h>
#include <stdio.h>

/**
 * A command line option description.
 *
 * There are two types of command line options, flags and pairs. The former
 * consists of only a key, while the latter consists of a name followed by a C
 * string value. An option is treated as a pair only if its `valueType`
 * property is non-NULL.
 *
 * @see arg_parse()
 */
typedef struct arg_Option {
    /// Alphanumeric ASCII character, or space if no key is wanted.
    char key;

    /// Option name.
    const char *name;

    /// Human-readable description of option.
    const char *description;

    /// Human-readable value type name, or `NULL` if option is flag.
    const char *valueType;
} arg_Option;

/**
 * The result of parsing some command line arguments.
 *
 * @see arg_parse()
 */
typedef struct arg_Result {
    /// Amount of remaining non-parsed arguments.
    int tailc;

    /// Pointer to first non-parsed argument.
    const char **tailv;

    /// If `false`, parsing stopped at unknown option now in `tailv[0]`.
    bool isOk;
} arg_Result;

/**
 * Prints `option` to `stream`.
 *
 * @param stream Target output stream.
 * @param option Pointer to option.
 */
void arg_fprintOption(FILE *stream, const arg_Option *option);

/**
 * Prints `options` to `stream`.
 *
 * @param stream Target output stream.
 * @param options Pointer to zero-terminated array of CLI options.
 */
void arg_fprintOptions(FILE *stream, const arg_Option options[]);

/**
 * @brief Parses `argc` and `argv` using `options` and writes any matches to
 * `out`.
 *
 * The `out` array referenced must be able to hold the same amount of C strings
 * as the length of `options`, excluding its zero-terminator. Arguments in
 * `argv` with matching options are inserted at the same offset in `out` as its
 * option has in `options`.
 *
 * Parsing deals with three kinds of arguments, namely, flags, pairs, and the
 * special stop argument. Parsing proceeds from the beginning of the provided
 * argument list `argv` until an argument is encountered that is neither a
 * known flag or a known pair. That and all subsequent arguments are considered
 * the argument tail, which is provided in the returned result. To cover for
 * the case of wanting to provide an argument value starting with one or two
 * dashes, the special stop argument, consisting of only two dashes (`--`), may
 * be provided to cause parsing to stop at the following argument.
 *
 * Options (i.e. flags and pairs) are accepted in two forms, the short and the
 * long form. The former consist of a dash followed by an option key (e.g.
 * `-o`), and the latter consist of two dashes followed by an option name (e.g.
 * `--option`).
 *
 * ## Example
 *
 * ```c
 * int main(const int argc, const char *argv[]) {
 *     const arg_Option options[] = {
 *         {'h', "help",    "Print help and exit."},      // Flag.
 *         {'o', "output",  "Path to output.", "OUTPUT"}, // Pair.
 *         {' ', "version", "Print version and exit."},   // Flag without key.
 *         {0}                                            // Zero-terminator.
 *     };
 *     const char *out[3] = {0}; // Zeroed array of same length as `options`.
 *
 *     // argc and argv are modified to skip program name entry.
 *     const arg_Result result = arg_parse(argc - 1, &argv[1], options, out);
 *     if (!result.isOk) {
 *         fprintf(stderr, "Unknown option: %s\n", result.tailv[0]);
 *         return EXIT_FAILURE;
 *     }
 *     // Positions of values in `out` correspond with positions in `options`.
 *     const bool help = out[0] != NULL; // Seen flags are set to "".
 *     const char *output = out[1];      // Seen pairs are set to their values.
 *     const bool version = out[2] != NULL;
 *
 *     return EXIT_SUCCESS;
 * }
 * ```
 *
 * @param argc Amount of elements in `argv`, excluding NULL-terminator.
 * @param argv Pointer to NULL-terminated list of C strings.
 * @param options Pointer to zero-terminated array of CLI options.
 * @param out Pointer to array to receive option values.
 * @return Parse result.
 *
 * @see arg_Option
 * @see arg_Result
 */
arg_Result arg_parse(int argc, const char **argv, const arg_Option options[],
                     const char **out);

#endif
