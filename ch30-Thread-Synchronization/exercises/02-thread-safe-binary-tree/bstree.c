#include "bstree.h"
#include <stddef.h> /* NULL */
#include <stdlib.h> /* malloc(), free() */
#include <string.h> /* strdup() */
#include <errno.h> /* errno */

/* Initialize tree */
int
initialize(bstree_s **tree)
{
	int s;
	bstree_s *t;

	/* Ensure a valid pointer was given */
	if (tree == NULL)
		return -1;

	/* Create tree and initialize fields */
	if ((t = malloc(sizeof(bstree_s))) == NULL)
		return -1;
	t->left = t->right = NULL;
	t->key = t->val = NULL;

	/* Create and initialize mutex */
	t->mtx = malloc(sizeof(pthread_mutex_t));
	if (t->mtx == NULL) {
		free(t);
		return -1;
	}
	s = pthread_mutex_init(t->mtx, NULL);
	if (s != 0) {
		free(t->mtx);
		free(t);
		return s;
	}
	*tree = t;
	return s;
}

void*
add(bstree_s *tree, char *key, void *val)
{
	/* Basic input check and lock mutex */
	if (tree == NULL || key == NULL || val == NULL || pthread_mutex_lock(tree->mtx) != 0)
		return NULL;

	bstree_s *node = tree;
	if (node->key != NULL) {
		bstree_s *parent;
		int cmp;
		do {
			cmp = strcmp(key, node->key);
			parent = node;
			if (cmp < 0)
				node = node->left;
			else if (cmp > 0)
				node = node->right;
			else
				break;
		} while (node != NULL);
		/* No matching key */
		if (node == NULL) {
			if (initialize(&node) != 0) {
				pthread_mutex_unlock(tree->mtx);
				return NULL;
			}
			if (cmp < 0)
				parent->left = node;
			else
				parent->right = node;
		}
	}

	/* Create and set key */
	if ((node->key = strdup(key)) == NULL) {
		pthread_mutex_unlock(tree->mtx);
		return NULL;
	}

	/* Set value */
	char *previousVal = node->val;
	node->val = val;

	/* Unlock */
	if (pthread_mutex_unlock(tree->mtx) != 0) {
		node->val = previousVal;		/* Restore value */
		return NULL;
	}

	if (previousVal == NULL)
		return val;
	else
		return previousVal;
}

/*
 * Find item in tree with given key.
 * If a match is found, stores result in value and returns true.
 * If not found or if an error occurs, returns false.
 */
bool lookup(bstree_s *tree, char *key, void **value)
{
	bool found;
	int s;

	/* Validate input */
	if (key == NULL || value == NULL) {
		errno = EINVAL;
		return false;
	}

	/* Not found */
	if (tree == NULL || tree->key == NULL)
		return false;
	
	/* Lock */
	if ((s = pthread_mutex_lock(tree->mtx)) != 0)
		return false;

	/* Compare and search */
	int cmp = strcmp(key, tree->key);
	if (cmp < 0) {
		found = lookup(tree->left, key, value);
	} else if (cmp > 0) {
		found = lookup(tree->right, key, value);
	} else {
		*value = tree->val;
		found = true;
	}

	/* Unlock */
	if ((s = pthread_mutex_unlock(tree->mtx)) != 0)
		return false;

	return found;
}
