#ifndef LIB_PVM_TYPES_H
#define LIB_PVM_TYPES_H

/**
 * PVM data types.
 *
 * @file
 */

#include <stdint.h>
#include <stdlib.h>

/// Indicates that some pvm_Function takes any amount of arguments when called.
#define PVM_FUNCTION_ARITY_ANY -1

typedef struct pvm_Function pvm_Function;

typedef struct pvm_Heap pvm_Heap;

typedef struct pvm_ValueArray pvm_ValueArray;
typedef struct pvm_ValueBytes pvm_ValueBytes;
typedef struct pvm_ValueClosure pvm_ValueClosure;
typedef struct pvm_ValueLazy pvm_ValueLazy;
typedef struct pvm_ValueLink pvm_ValueLink;
typedef struct pvm_ValueNumber pvm_ValueNumber;
typedef struct pvm_ValueSymbol pvm_ValueSymbol;

typedef struct pvm_Value pvm_Value;


/**
 * A named function of known arity.
 */
struct pvm_Function {
    /// Function name.
    const char *name;

    /// Function parameter count, or PVM_FUNCTION_ARITY_ANY if variadic.
    intptr_t arity;

    /// Actual function pointer.
    pvm_Value (*pointer)(pvm_Value *);
};

/**
 * A block of memory containing pvm_Value objects.
 */
struct pvm_Heap {
    /// Heap capacity, in bytes.
    size_t capacity;

    /// Heap length, in bytes.
    size_t length;

    /// Pointer to first byte in heap memory.
    uint8_t *buffer;

    /// File handle, if heap is associated with a file.
    int handle;
};

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

    /// Offset from beginning of some memory block owned by referenced heap.
    size_t offset;
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
 * A PVM value.
 *
 * Implemented as a tagged union. Only the union value named by the value kind
 * may be safely used.
 */
struct pvm_Value {
    /// Value kind.
    enum {
        PVM_VALUE_UNDEFINED,
        PVM_VALUE_BYTES,
        PVM_VALUE_NUMBER,
        PVM_VALUE_SYMBOL,
        PVM_VALUE_CLOSURE,
        PVM_VALUE_ARRAY,
        PVM_VALUE_LINK,
        PVM_VALUE_LAZY,
    } kind;
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
