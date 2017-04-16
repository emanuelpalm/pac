#ifndef LIB_RVM_NODE_H
#define LIB_RVM_NODE_H

/// RVM node type and utilities.
///
/// \file

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "error.h"
#include "function.h"

/// Bit mask for extracting rvm_NodeKind from uint64_t flags.
#define RVM_NODE_FLAGS_KIND 0x0000000000000007

/// Bit mask for extracting rvm_Node index from uint64_t flags.
#define RVM_NODE_FLAGS_INDEX 0x0ffffffffffffff8

/// Indicates that some rvm_Node lacks an index.
#define RVM_NODE_INDEX_NONE 0

typedef struct rvm_Heap rvm_Heap;

typedef struct rvm_NodeArray rvm_NodeArray;
typedef struct rvm_NodeBytes rvm_NodeBytes;
typedef struct rvm_NodeClosure rvm_NodeClosure;
typedef struct rvm_NodeLazy rvm_NodeLazy;
typedef struct rvm_NodeLink rvm_NodeLink;
typedef struct rvm_NodeNumber rvm_NodeNumber;
typedef struct rvm_NodeSymbol rvm_NodeSymbol;

typedef struct rvm_Node rvm_Node;
typedef struct rvm_NodeResult rvm_NodeResult;

/// A sequence of rvm_Node objects of known length.
struct rvm_NodeArray {
    /// Amount of nodes in sequence.
    size_t length;

    /// Pointer to first node in sequence.
    const rvm_Node *nodes;
};

/// A sequence of bytes of known length.
struct rvm_NodeBytes {
    /// Amount of bytes in sequence.
    size_t length;

    /// Pointer to first byte in sequence.
    const uint8_t *bytes;
};

/// A function closure.
struct rvm_NodeClosure {
    /// Pointer to function.
    const rvm_Function *function;

    /// Enclosed node, if any.
    const rvm_Node *node;
};

/// An rvm_Node yet to be loaded.
struct rvm_NodeLazy {
    /// Reference to heap containing node not yet loaded.
    const rvm_Heap *heap;
};

/// A link joining two rvm_Node objects.
struct rvm_NodeLink {
    /// Link first node.
    const rvm_Node *head;

    /// Link second node.
    const rvm_Node *tail;
};

/// A numeric node.
struct rvm_NodeNumber {
    /// Number integer node.
    int64_t integer;
};

/// A named symbol table reference.
struct rvm_NodeSymbol {
    /// Amount of bytes in symbol name.
    size_t length;

    /// Pointer to first byte in symbol name.
    const uint8_t *bytes;
};

/// Identifies the kind of some rvm_Node.
///
/// The ordinal of each kind must be fit inside the RVM_NODE_FLAGS_KIND
/// bit mask.
typedef enum rvm_NodeKind {
    RVM_NODE_UNDEFINED = 0x0,
    RVM_NODE_BYTES = 0x1,
    RVM_NODE_NUMBER = 0x2,
    RVM_NODE_SYMBOL = 0x3,
    RVM_NODE_CLOSURE = 0x4,
    RVM_NODE_ARRAY = 0x5,
    RVM_NODE_LINK = 0x6,
    RVM_NODE_LAZY = 0x7,
} rvm_NodeKind;

/// An RVM tree node.
///
/// Each node has a kind, an index, and a body whose fields vary depending on
/// the kind.
///
/// ## Indexed Nodes
///
/// Nodes that originate from RVM heaps all have an index node, which uniquely
/// identifies its position within that memory. Such nodes are referred to as
/// being indexed.
struct rvm_Node {
    /// Node flags.
    ///
    /// Contains bit mask fields accessible using the following masks:
    /// - RVM_NODE_FLAGS_KIND
    /// - RVM_NODE_FLAGS_INDEX
    ///
    /// \see rvm_getNodeKind()
    /// \see rvm_getNodeIndex()
    uint64_t flags;

    /// Node body.
    ///
    /// Only the union field corresponding to the current kind may be safely
    /// used.
    union {
        rvm_NodeBytes bytes;
        rvm_NodeNumber number;
        rvm_NodeSymbol symbol;
        rvm_NodeClosure closure;
        rvm_NodeArray array;
        rvm_NodeLink link;
        rvm_NodeLazy lazy;
    } as;
};

///
struct rvm_NodeResult {
    bool ok;

    union {
        rvm_Error error;
        rvm_Node node;
    } as;
};

/// Resolves rvm_NodeKind of given node.
///
/// \param node Inspected node.
/// \returns     Node kind.
///
/// \see rvm_Node
static inline rvm_NodeKind rvm_getNodeKind(rvm_Node *node) {
    return (rvm_NodeKind)(node->flags & RVM_NODE_FLAGS_KIND);
}

/// Resolves index of given node.
///
/// \param node Inspected node.
/// \returns     Node index, or RVM_NODE_INDEX_NONE if no such is available.
///
/// \see rvm_Node
static inline uint64_t rvm_getNodeIndex(rvm_Node *node) {
    return node->flags & RVM_NODE_FLAGS_INDEX;
}

#endif
