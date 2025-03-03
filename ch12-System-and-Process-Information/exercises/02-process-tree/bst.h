#ifndef BST_ADT_H
#define BST_ADT_H

/* Implements a binary search tree adt */

typedef struct bst_type *BST;

/* Comparison function */
typedef int (*BSTCmpFn)(const void*,const void*);
typedef void (*BSTWalkAction)(BST, const void*, void*);

/* Creates a BST object */
BST bst_create(void);

/* Destroys a BST object */
void bst_destroy(BST t);

/* Adds or updates key-value pair in BST */
void *bst_put(BST t, const void *key, void *val, BSTCmpFn f);

/* Finds a value corresponding to the given key in the BST */
void *bst_get(const BST t, const void *key, BSTCmpFn f);

/* Performs the given action on all nodes in the BST */
void bst_walk(BST t, BSTWalkAction action);

#endif
