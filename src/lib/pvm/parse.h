#ifndef LIB_PVM_PARSE_H
#define LIB_PVM_PARSE_H

/**
 * Compiler parsing utilities.
 *
 * @file
 */

#include "types.h"
#include <stddef.h>

/**
 * Parses given ASCII string into node tree.
 *
 * @param string Pointer to C string to parse.
 * @param length Length of string in bytes.
 * @returns Pointer to compiler node tree, or NULL if memory allocation failed.
 */
pvm_Value *pvm_parse(const char *string, size_t length);


#endif
