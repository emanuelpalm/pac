#ifndef LIB_PVM_HEAP_H
#define LIB_PVM_HEAP_H

/**
 * PVM heap type and utilities.
 *
 * @file
 */

#include <stdint.h>
#include <stdlib.h>

typedef struct pvm_Heap pvm_Heap;

/**
 * Represents a block of contiguous memory containing pvm_Value objects and
 * their associated data.
 */
struct pvm_Heap {
    /// Capacity of heap, in bytes.
    size_t capacity;

    /// Amount of currently used heap bytes, from beginning.
    size_t length;

    /// Pointer to first byte in heap.
    uint8_t *bytes;

    /// File associated with heap, if any.
    struct {
        /// System file handle, or -1 if none.
        int descriptor;

        /// File path, or NULL if none.
        const char *path;
    } file;
};

#endif
