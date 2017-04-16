#ifndef LIB_RVM_HEAP_H
#define LIB_RVM_HEAP_H

/// RVM heap type and utilities.
///
/// \file

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "value.h"

typedef struct rvm_Heap rvm_Heap;
typedef struct rvm_HeapResult rvm_HeapResult;

/// Represents a block of contiguous memory containing rvm_Value objects and
/// associated data.
///
/// ## Destruction
///
/// Once no longer used, heaps must be freed using pvm_freeHeap().
///
/// \see rvm_bufferAsHeap()
/// \see rvm_fileAsHeap()
/// \see rvm_fileIntoHeap()
struct rvm_Heap {
    /// Size of heap, in bytes.
    uint64_t length;

    /// Current heap revision.
    uint64_t revision;

    /// Pointer to private heap data.
    void *internal;

    /// Closes given heap, freeing up any resources held.
    ///
    /// \param self This heap.
    ///
    /// \see rvm_freeHeap()
    void (*free)(rvm_Heap *self);

    rvm_Error (*get)(rvm_Heap *self, rvm_Value *out, const uint64_t revision);

    /// Sets heap value, creating a new revision.
    ///
    /// \param self  This heap.
    /// \param value Value to set.
    rvm_Error (*set)(rvm_Heap *self, const rvm_Value value);

    /// Synchronizes heap contents.
    ///
    /// When the function returns, any prior heap changes are guaranteed to be
    /// committed to any underlying medium. Note that the calling thread is
    /// suspended until the operation completes.
    ///
    /// \param self This heap.
    rvm_Error (*sync)(rvm_Heap *self);
};

/// Carries result of an attempt to create a new heap.
///
/// \see rvm_bufferAsHeap()
/// \see rvm_fileAsHeap()
/// \see rvm_fileIntoHeap()
struct rvm_HeapResult {
    /// Indicates whether creation was successful.
    bool ok;

    union {
        /// If creation was unsuccessful, carries indication of cause.
        rvm_Error error;

        /// If creation was successful, carries created heap.
        rvm_Heap heap;
    } as;
};

/// Initializes heap with provided buffer of length bytes as memory.
///
/// Ownership is not taken of the given buffer, meaning that it will not be
/// released when the heap is freed. As the buffer is not owned, no attempt
/// will be made to resize it if more space would be required.
///
/// \param buffer Target memory buffer.
/// \param length Length of memory buffer, in bytes.
/// \returns      Error object, indicating any issues.
///
/// \see rvm_Heap
/// \see rvm_freeHeap()
rvm_HeapResult rvm_bufferAsHeap(uint8_t *buffer, const size_t length);

/// Initializes heap using provided file.
///
/// Ownership is not taken of the given file, meaning it will not be closed
/// when the heap is freed.
///
/// \param heap Pointer to uninitialized heap structure to initialize.
/// \param file File to use.
/// \returns    Error object, indicating any issues.
///
/// \see rvm_Heap
/// \see rvm_freeHeap()
rvm_HeapResult rvm_fileAsHeap(FILE *file);

/// Initializes heap using file at provided path.
///
/// Takes ownership of the given file, meaning it will be closed when the heap
/// is freed.
///
/// \param heap Pointer to uninitialized heap structure to initialize.
/// \param path Path to file to open.
/// \returns    Error object, indicating any issues.
///
/// \see rvm_Heap
/// \see rvm_freeHeap()
rvm_HeapResult rvm_fileIntoHeap(FILE *file);

/// Gets heap value associated with given revision.
///
/// \param self     This heap.
/// \param out      Pointer to value receiver.
/// \param revision Target heap revision.
/// \returns        Error object, indicating any issues.


/// Destroys given heap, freeing up any resources held.
///
/// Any pending changes are guaranteed to be synchronized to any associated
/// medium before freeing completes.
///
/// \param self Heap to free.
static inline void rvm_freeHeap(rvm_Heap *heap) {
    assert(heap != NULL);
    assert(heap->free != NULL);

    heap->free(heap);
}

#endif
