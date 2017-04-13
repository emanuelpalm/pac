#include <stdlib.h>
#include <string.h>
#include "../../lib/rvm/error.h"
#include "../../lib/unit/unit.h"

void shouldUseKindAndMessageAsError(unit_T *t) {
    const rvm_Error error = rvm_asError(RVM_ERROR_USER, "Error A");
    UNIT_ASSERT_EQU(t, RVM_ERROR_USER, rvm_getErrorKind(error));
    UNIT_ASSERT_EQS(t, "Error A", error.message);

    // Should not free anything.
    rvm_freeError(error);
}

void shouldTurnKindAndMessageIntoError(unit_T *t) {
    const char *message = "Error B";
    char *buffer = calloc(strlen(message) + 1, 1);
    UNIT_ASSERT(t, buffer != NULL);
    strcpy(buffer, message);

    const rvm_Error error = rvm_intoError(RVM_ERROR_USER, buffer);
    UNIT_ASSERT_EQU(t, RVM_ERROR_USER, rvm_getErrorKind(error));
    UNIT_ASSERT_EQS(t, "Error B", error.message);

    // Should casuse buffer to be freed.
    rvm_freeError(error);
}

void shouldCreateNewErrorFromKindAndMessage(unit_T *t) {
    const rvm_Error error = rvm_newError(RVM_ERROR_USER, "Error C");
    UNIT_ASSERT_EQU(t, RVM_ERROR_USER, rvm_getErrorKind(error));
    UNIT_ASSERT_EQS(t, "Error C", error.message);

    // Should free copy of "Error" string provided to rvm_newError().
    rvm_freeError(error);
}

void rvm_error(unit_S *s) {
    unit_test(s, shouldUseKindAndMessageAsError);
    unit_test(s, shouldTurnKindAndMessageIntoError);
    unit_test(s, shouldCreateNewErrorFromKindAndMessage);
}

