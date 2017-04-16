#ifndef LIB_RVM_EVALUATE_H
#define LIB_RVM_EVALUATE_H

/// RVM value evaluation utilities.
///
/// \file

#include <stdint.h>
#include "error.h"
#include "value.h"

typedef struct rvm_Result rvm_Result;

struct rvm_Result {
    rvm_Error error;
    rvm_Value value;
};

rvm_Result rvm_evaluate(rvm_Value *value);

#endif
