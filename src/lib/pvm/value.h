#ifndef LIB_PVM_VALUE_H
#define LIB_PVM_VALUE_H

/**
 * PVM value type and utilities.
 *
 * @file
 */

#include "function.h"
#include "heap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/// Bit mask for extracting pvm_ValueKind from uint64_t flags.
#define PVM_VALUE_FLAGS_KIND 0x0000000000000007

/// Bit mask for extracting pvm_Value index from uint64_t flags.
#define PVM_VALUE_FLAGS_INDEX 0x0ffffffffffffff8

typedef struct pvm_ValueArray pvm_ValueArray;
typedef struct pvm_ValueBytes pvm_ValueBytes;
typedef struct pvm_ValueClosure pvm_ValueClosure;
typedef struct pvm_ValueLazy pvm_ValueLazy;
typedef struct pvm_ValueLink pvm_ValueLink;
typedef struct pvm_ValueNumber pvm_ValueNumber;
typedef struct pvm_ValueSymbol pvm_ValueSymbol;

typedef enum pvm_ValueKind pvm_ValueKind;
typedef struct pvm_Value pvm_Value;

/**
 * A sequence of pvm_Value objects of known length.
 */
struct pvm_ValueArray {
    /// Amount of values in sequence.
    size_t length;

    /// Pointer to first value in sequence.
    const pvm_Value *values;
};

/**
 * A sequence of bytes of known length.
 */
struct pvm_ValueBytes {
    /// Amount of bytes in sequence.
    size_t length;

    /// Pointer to first byte in sequence.
    const uint8_t *bytes;
};

/**
 * A function closure.
 */
struct pvm_ValueClosure {
    /// Pointer to function.
    const pvm_Function *function;

    /// Enclosed value, if any.
    const pvm_Value *value;
};

/**
 * A pvm_Value yet to be loaded.
 */
struct pvm_ValueLazy {
    /// Reference to heap containing value not yet loaded.
    const pvm_Heap *heap;
};

/**
 * A link joining two pvm_Value objects.
 */
struct pvm_ValueLink {
    /// Link first value.
    const pvm_Value *head;

    /// Link second value.
    const pvm_Value *tail;
};

/**
 * A numeric value.
 */
struct pvm_ValueNumber {
    /// Number integer value.
    int64_t integer;
};

/**
 * A named symbol table reference.
 */
struct pvm_ValueSymbol {
    /// Amount of bytes in symbol name.
    size_t length;

    /// Pointer to first byte in symbol name.
    const uint8_t *bytes;
};

/**
 * Identifies the kind of some pvm_Value.
 *
 * The ordinal of each kind must be fit inside the PVM_VALUE_FLAGS_KIND
 * bitmask.
 */
enum pvm_ValueKind {
    PVM_VALUE_UNDEFINED = 0,
    PVM_VALUE_BYTES = 1,
    PVM_VALUE_NUMBER = 2,
    PVM_VALUE_SYMBOL = 3,
    PVM_VALUE_CLOSURE = 4,
    PVM_VALUE_ARRAY = 5,
    PVM_VALUE_LINK = 6,
    PVM_VALUE_LAZY = 7,
};

/**
 * A PVM value.
 *
 * Values are the nodes that make up a PVM state tree. Each node has a kind,
 * an index, and a body whose fields vary depending on the kind.
 *
 * ### Indexed Values
 *
 * Values that originate from persistent memory all have an index value, which
 * uniquely identifies its position within that memory. Such values are
 * referred to as being indexed.
 */
struct pvm_Value {
    /// Value flags.
    ///
    /// Contains bitmask fields accessible using the following masks:
    /// - PVM_VALUE_FLAGS_KIND
    /// - PVM_VALUE_FLAGS_INDEX
    /// - PVM_VALUE_FLAGS_MARK
    uint64_t flags;

    /// Value data.
    union {
        pvm_ValueBytes bytes;
        pvm_ValueNumber number;
        pvm_ValueSymbol symbol;
        pvm_ValueClosure closure;
        pvm_ValueArray array;
        pvm_ValueLink link;
        pvm_ValueLazy lazy;
    } as;
};

#endif
