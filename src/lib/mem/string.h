#ifndef LIB_MEM_STRING_H
#define LIB_MEM_STRING_H

/// C string utilities.
///
/// \file

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/// Creates new string by copying given string.
///
/// The provided string must not be NULL, and must be 0-terminated, or the
/// function will cause undefined behavior.
///
/// The returned string must be freed using free().
static inline char *mem_newString(const char *string) {
    assert(string != NULL);

    char *buf = NULL;
    const size_t len = strlen(string) + 1;
    if ((buf = malloc(len)) != NULL) {
        strncpy(buf, string, len);
    }
    return buf;
}

#endif
