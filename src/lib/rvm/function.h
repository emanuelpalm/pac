#ifndef LIB_RVM_FUNCTION_H
#define LIB_RVM_FUNCTION_H

/// RVM function type and utilities.
///
/// \file

#include <stdint.h>

typedef struct rvm_Function rvm_Function;
typedef struct rvm_Node rvm_Node;

/// A named node function of known arity.
struct rvm_Function {
    /// Function name.
    const char* name;

    /// Function parameter count.
    intptr_t arity;

    /// Pointer to actual C function.
    rvm_Node (*pointer)(rvm_Node *);
};

#endif
