#ifndef LIB_RVM_VALUE_H
#define LIB_RVM_VALUE_H

/// RVM value type and utilities.
///
/// \file

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "function.h"
#include "heap.h"

/// Bit mask for extracting rvm_ValueKind from uint64_t flags.
#define RVM_VALUE_FLAGS_KIND 0x0000000000000007

/// Bit mask for extracting rvm_Value index from uint64_t flags.
#define RVM_VALUE_FLAGS_INDEX 0x0ffffffffffffff8

/// Indicates that some rvm_Value lacks an index.
#define RVM_VALUE_INDEX_NONE 0

typedef struct rvm_ValueArray rvm_ValueArray;
typedef struct rvm_ValueBytes rvm_ValueBytes;
typedef struct rvm_ValueClosure rvm_ValueClosure;
typedef struct rvm_ValueLazy rvm_ValueLazy;
typedef struct rvm_ValueLink rvm_ValueLink;
typedef struct rvm_ValueNumber rvm_ValueNumber;
typedef struct rvm_ValueSymbol rvm_ValueSymbol;

typedef struct rvm_Value rvm_Value;

/// A sequence of rvm_Value objects of known length.
struct rvm_ValueArray {
    /// Amount of values in sequence.
    size_t length;

    /// Pointer to first value in sequence.
    const rvm_Value *values;
};

/// A sequence of bytes of known length.
struct rvm_ValueBytes {
    /// Amount of bytes in sequence.
    size_t length;

    /// Pointer to first byte in sequence.
    const uint8_t *bytes;
};

/// A function closure.
struct rvm_ValueClosure {
    /// Pointer to function.
    const rvm_Function *function;

    /// Enclosed value, if any.
    const rvm_Value *value;
};

/// An rvm_Value yet to be loaded.
struct rvm_ValueLazy {
    /// Reference to heap containing value not yet loaded.
    const rvm_Heap *heap;
};

/// A link joining two rvm_Value objects.
struct rvm_ValueLink {
    /// Link first value.
    const rvm_Value *head;

    /// Link second value.
    const rvm_Value *tail;
};

/// A numeric value.
struct rvm_ValueNumber {
    /// Number integer value.
    int64_t integer;
};

/// A named symbol table reference.
struct rvm_ValueSymbol {
    /// Amount of bytes in symbol name.
    size_t length;

    /// Pointer to first byte in symbol name.
    const uint8_t *bytes;
};

/// Identifies the kind of some rvm_Value.
///
/// The ordinal of each kind must be fit inside the RVM_VALUE_FLAGS_KIND
/// bitmask.
typedef enum rvm_ValueKind {
    RVM_VALUE_UNDEFINED = 0x0,
    RVM_VALUE_BYTES = 0x1,
    RVM_VALUE_NUMBER = 0x2,
    RVM_VALUE_SYMBOL = 0x3,
    RVM_VALUE_CLOSURE = 0x4,
    RVM_VALUE_ARRAY = 0x5,
    RVM_VALUE_LINK = 0x6,
    RVM_VALUE_LAZY = 0x7,
} rvm_ValueKind;

/// An RVM value.
///
/// Values are the nodes that make up a RVM state tree. Each node has a kind,
/// an index, and a body whose fields vary depending on the kind.
///
/// ## Indexed Values
///
/// Values that originate from persistent memory all have an index value, which
/// uniquely identifies its position within that memory. Such values are
/// referred to as being indexed.
struct rvm_Value {
    /// Value flags.
    ///
    /// Contains bitmask fields accessible using the following masks:
    /// - RVM_VALUE_FLAGS_KIND
    /// - RVM_VALUE_FLAGS_INDEX
    ///
    /// \see rvm_getValueKind()
    /// \see rvm_getValueIndex()
    uint64_t flags;

    /// Value body.
    ///
    /// Only the union field corresponding to the current kind may be safely
    /// used.
    union {
        rvm_ValueBytes bytes;
        rvm_ValueNumber number;
        rvm_ValueSymbol symbol;
        rvm_ValueClosure closure;
        rvm_ValueArray array;
        rvm_ValueLink link;
        rvm_ValueLazy lazy;
    } as;
};

/// Resolves rvm_ValueKind of given value.
///
/// \param value Inspected value.
/// \returns     Value kind.
///
/// \see rvm_Value
static inline rvm_ValueKind rvm_getValueKind(rvm_Value *value) {
    return value->flags & RVM_VALUE_FLAGS_KIND;
}

/// Resolves index of given value.
///
/// \param value Inspected value.
/// \returns     Value index, or RVM_VALUE_INDEX_NONE if no such is available.
///
/// \see rvm_Value
static inline uint64_t rvm_getValueIndex(rvm_Value *value) {
    return value->flags & RVM_VALUE_FLAGS_INDEX;
}

#endif
