#ifndef LIB_PVM_FUNCTION_H
#define LIB_PVM_FUNCTION_H

/// PVM function type and utilities.
///
/// \file

#include <stdint.h>

/// Indicates that some pvm_Function takes any amount of arguments when called.
#define PVM_FUNCTION_ARITY_ANY -1

typedef struct pvm_Function pvm_Function;
typedef struct pvm_Value pvm_Value;

/// A named function of known arity.
struct pvm_Function {
    /// Function name.
    const char *name;

    /// Function parameter count, or PVM_FUNCTION_ARITY_ANY if variadic.
    intptr_t arity;

    /// Actual function pointer.
    pvm_Value (*pointer)(pvm_Value *);
};

#endif

