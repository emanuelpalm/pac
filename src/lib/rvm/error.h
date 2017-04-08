#ifndef LIB_RVM_ERROR_H
#define LIB_RVM_ERROR_H

/// RVM error utilities.
///
/// \file

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/// Bit mask for extracting free bit from uint64_t flags.
#define RVM_ERROR_FLAGS_FREE 0x8000000000000000

/// Bit mask for extracting rvm_ErrorKind from uint64_t flags.
#define RVM_ERROR_FLAGS_KIND 0x00000000000000ff

typedef struct rvm_Error rvm_Error;

/// Identifies the kind of some rvm_Error.
///
/// The ordinal of each kind must be fit inside the RVM_ERROR_FLAGS_KIND
/// bitmask.
typedef enum rvm_ErrorKind {
    RVM_ERROR_NONE = 0x00,
    RVM_ERROR_NOMEMORY = 0x01,
    RVM_ERROR_USER = 0xff,
} rvm_ErrorKind;

/// An RVM error.
///
/// Errors are used to indicate how some RVM operation failed, unless it was
/// successful.
///
/// ## Destruction
///
/// Once no longer required, each error must be provided to rvm_freeError() to
/// free up any unused resources, unless the kind of the error is
/// RVM_ERROR_NONE.
///
/// \see rvm_freeError()
struct rvm_Error {
    /// Error flags.
    ///
    /// Contains bitmask fields accessible using the following mask:
    /// - RVM_ERROR_FLAGS_KIND
    ///
    /// \see rvm_getErrorKind()
    uint64_t flags;

    /// Error message.
    ///
    /// The message serves as a complement to the error kind, and is language
    /// agnostic to the furthest extent possible. If anything at all, it could
    /// contain a filename, memory address, etc. The content of the message
    /// should be obvious just by looking at the name of the error kind.
    ///
    /// User errors are naturally not required to be language agnostic, as
    /// these are provided by the user, and not the RIM system.
    char *message;
};

/// Uses given kind and message as error.
///
/// The created rvm_Error object does not take ownership of the provided
/// message, meaning it will never be provided to free() even if the error is
/// given to rvm_freeError().
///
/// It is safe to provide a `NULL` message.
static inline rvm_Error rvm_asError(rvm_ErrorKind kind, const char *message) {
    return (rvm_Error){
        .flags = kind, .message = (char *)message,
    };
}

/// Converts given kind and message into error.
///
/// The created rvm_Error object takes ownerhip of the provided message, which
/// means that it will be provided to free() if the error is given to
/// rvm_freeError().
///
/// It is safe to provide a `NULL` message.
static inline rvm_Error rvm_toError(rvm_ErrorKind kind, char *message) {
    return (rvm_Error){
        .flags = kind | RVM_ERROR_FLAGS_FREE, .message = message,
    };
}

/// Creates new error by copying given kind and message.
///
/// It is safe to provide a `NULL` message.
static inline rvm_Error rvm_newError(rvm_ErrorKind kind, char *message) {
    char *buf = NULL;
    if (message != NULL) {
        const size_t len = strlen(message) + 1;
        if ((buf = malloc(len)) != NULL) {
            strncpy(buf, message, len);
        }
    }
    return rvm_toError(kind, buf);
}

/// Frees any dynamically allocated resources held by given error.
///
/// \param error Destroyed error.
///
/// \see rvm_Error
static inline void rvm_freeError(rvm_Error error) {
    if (error.flags & RVM_ERROR_FLAGS_FREE) {
        free(error.message);
    }
}

/// Resolves rvm_ErrorKind of given error.
///
/// \param error Inspected error.
/// \returns     Error kind.
///
/// \see rvm_Error
static inline rvm_ErrorKind rvm_getErrorKind(rvm_Error error) {
    return error.flags & RVM_ERROR_FLAGS_KIND;
}

#endif
