# Exercise 30.2

Implement a set of thread-safe functions that update and search an unbalanced binary tree.
This library should include functions (with the obvious purposes) of the following form;

```c
initialize(tree);
add(tree, char *key, void *value);
delete(tree, char *key);
Boolean lookup(char *key, void **value);
```

In the above prorotype, *tree* is a structure that points to the root of the tree
(you need to define a suitable structure for this purpose). Each element of the tree
holds a key-value pair. You will also need to define the structure for each element to
include a mutex that protects that element so that only one thread at a time can
access it. The `initialize()`, `add()`, and `lookup()` functions are relatively simple to
implement. The `delete()` operation requires a little more effort.

> Removing the need to maintain a balanced tree greatly simplifies the locking requirements
of the implementation, but carries the risk that certain patterns of input would result in
a tree that performs poorly. Maintaining a balanced tree necessitates moving nodes between
subtrees during the `add()` and `delete()` operations, which requires much more complex
locking strategies.

## Solution

I began by creating an interactive program `main.c` for adding and deleting key-value pairs.
It expects at least one key-value pair to start. The program was my means for testing my
implementation. I implemented `initialize()`, `add()`, and `lookup()`, but not `delete()`.

Initially I implemented `add()` recursively, but struggled to think of a way to communicate errors
to the client. Therefore I switched to a serial implementation. I tried
paying special attention to memory leaks. The approach I took was that on success, `add()` works
like a *put*, meaning that if a node on the tree exists with the specified key, and it has an
associated non `NULL` value, then that value is returned to the client. This way they can decide
if they wish to deallocate it themselves or do something else with it. Otherwise it returns the
key provided by the user. On error it returns `NULL`.

I struggled with deciding when I should lock the mutex for the nodes. In my implementation, I only
locked the mutex for the root of the tree, because I determined that if a thread cannot use
the mutex for the root, then they certainly will not be able to use the mutex for any other node.
I need to revisit this assumption later on.

I did not implement `delete()`, and I hope to return to give this a try.

## Compiling Running

```bash
# Compile, enabling posix threads
gcc main.c bstree.c -pthread

# Provide at least one key-value pair as a command-line argument.
# Here I give 3: (name, sergio), (last, garcia), (age, 28)
./a.out name sergio last garcia age 28
```
