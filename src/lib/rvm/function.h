#ifndef LIB_RVM_FUNCTION_H
#define LIB_RVM_FUNCTION_H

/// RVM function type and utilities.
///
/// \file

#include <stdint.h>

/// Indicates that some rvm_Function takes any amount of arguments when called.
#define RVM_FUNCTION_ARITY_ANY -1

typedef struct rvm_Function rvm_Function;
typedef struct rvm_Value rvm_Value;

/// A named function of known arity.
struct rvm_Function {
    /// Function name.
    const char *name;

    /// Function parameter count, or RVM_FUNCTION_ARITY_ANY if variadic.
    intptr_t arity;

    /// Actual function pointer.
    rvm_Value (*pointer)(rvm_Value *);
};

#endif

