#ifndef LIB_RVM_HEAP_H
#define LIB_RVM_HEAP_H

/// RVM heap type and utilities.
///
/// @file

#include "error.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct rvm_Heap rvm_Heap;

/// Represents a block of contiguous memory containing rvm_Value objects and
/// their associated data.
///
/// ## Initalization
///
/// Heaps are first allocated, typically on the stack, and then initialized by
/// mounting them via pvm_mount() or pvm_mountTemporary().
///
/// ## Destruction
///
/// Once no longer required, each initialized heap must be unmounted using the
/// pvm_unmount() function.
///
/// \see rvm_mount()
/// \see rvm_mountTemporary()
/// \see rvm_unmount()
struct rvm_Heap {
    /// Capacity of heap, in bytes.
    size_t capacity;

    /// Amount of currently used heap bytes, from beginning.
    size_t length;

    /// Pointer to first byte in heap.
    uint8_t *bytes;

    /// File associated with heap, if any.
    struct {
        /// System file handle, or -1 if none.
        int descriptor;

        /// File path, or NULL if none.
        const char *path;
    } file;
};

/// Attempts to mount file at given path as an RVM heap with given capacity.
///
/// The file is created if it does not exist.
///
/// The given capacity determines how many bytes of the process address space
/// are allocated for the heap. The provided capacity will be rounded up to the
/// nearest useful size, which on most systems will be a multiple of the system
/// page size. The capacity may be larger than the actual storage capacity of
/// the computer, as only the portions of the heap that are actually used are
/// stored to disk.
///
/// \param heap     Pointer to uninitialized heap structure to initialize.
/// \param path     Path to file to mount.
/// \param capacity Size of process address space to request for heap.
/// \returns        Error object, indicating any issues.
///
/// \see rvm_Heap
/// \see rvm_unmount()
rvm_Error rvm_mount(rvm_Heap *heap, const char *path, const size_t capacity);

/// Attempts to mount a temporary RVM heap with given capacity.
///
/// The capacity is on typical system limited to the amount of primary and swap
/// memory the operating systems is allowed to provide to processes.
///
/// \param heap     Pointer to uninitialized heap structure to initialize.
/// \param capacity Size of process address space to request for heap.
/// \returns        Error object, indicating any issues.
///
/// \see rvm_Heap
/// \see rvm_unmount()
rvm_Error rvm_mountTemporary(rvm_Heap *heap, const size_t capacity);

/// Unmounts given RVM heap, freeing up any resources held.
///
/// \param heap Heap to unmount.
///
/// \see rvm_Heap
void rvm_unmount(rvm_Heap *heap);

#endif
