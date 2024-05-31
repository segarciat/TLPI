#include <stdbool.h> /* bool */
#include <pthread.h> /* pthread_mutex_t */

typedef struct _bstree {
	char *key;
	void *val;
	pthread_mutex_t *mtx;
	pthread_cond_t  *cnd;

	struct _bstree *left;
	struct _bstree *right;
} bstree_s;

/* Creates a tree initializes it */
int initialize(bstree_s **tree);

/*
 * Must initialize tree before calling this function.
 *
 * Adds a new node on the tree corresponding to the given key.
 * If a node with a matching key exists, assigns the new value and returns the old one; if
 * the existing node had no associated value, then it returns the given value.
 *
 * If no node with a matching key exists, allocates memory for the key and the new node,
 * and assigns they key-value pair.
 *
 * If an error occurs, returns NULL.
 */
void *add(bstree_s *tree, char *key, void *value);

/* Delete item given tree with associated key */
// void *delete(bstree_s tree, char *key);

/*
 * Must initialize tree before calling this function
 * 
 * Find item in tree with given key, and stores result in value if found.
 * If not found or an error occurs, return NULL. Check errno for errors.
 */
bool lookup(bstree_s *tree, char *key, void **value);
