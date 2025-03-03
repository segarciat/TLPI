#include <stdlib.h> /* malloc */
#include <stddef.h> /* NULL */
#include <errno.h>
#include "bst.h"

struct bst_type {
    struct bst_node *root;
};

struct bst_node {
    const void *key;
    void *val;
    struct bst_node *left;
    struct bst_node *right;
};

BST bst_create(void)
{
    BST t = malloc(sizeof*(t));
    if (t == NULL)
        return NULL;
    t->root = NULL;
    return t;
}

void *bst_get(const BST t, const void *k, BSTCmpFn pcmpf)
{
    if (t == NULL)
        return NULL;

    struct bst_node *n = t->root;
    while (n != NULL) {
        int cmp = (*pcmpf)(k, n->key);
        if (cmp < 0)
            n = n->left;
        else if (cmp > 0)
            n = n->right;
        else
            return n->val;
    }
    return NULL;
}

void *bst_put(BST t, const void *k, void *v, BSTCmpFn pcmpf)
{
    if (t == NULL) {
        errno = EINVAL;
        return NULL;
    }

    struct bst_node *parent = NULL;
    struct bst_node *current = t->root;
    int cmp = 0;
    // Find node in tree, if it exists
    while (current != NULL) {
        cmp = (*pcmpf)(k, current->key);
        parent = current;
        if (cmp < 0)
            current = current->left;
        else if (cmp > 0)
            current = current->right;
        else {
            // Existing node
            void *oldVal = current->val;
            current->val = v;
            return oldVal;
        }
    }
    // Create node
    struct bst_node *newNode = malloc(sizeof(*newNode));
    if (newNode == NULL)
        return NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->key = k;
    newNode->val = v;

    // Add node to tree
    if (parent == NULL) {
        t->root = newNode;
    } else if (cmp < 0) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
    return NULL;
}

static void bst_walk_helper(BST t, struct bst_node *n, BSTWalkAction action)
{
    if (n != NULL) {
        bst_walk_helper(t, n->left, action);
        action(t, n->key, n->val);
        bst_walk_helper(t, n->right, action);
    }
}

void bst_walk(BST t, BSTWalkAction action)
{
    bst_walk_helper(t, t->root, action);
}

