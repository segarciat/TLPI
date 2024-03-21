#include "process.h"
#include <stdlib.h> /* malloc */
#include <string.h> /* strlen */

struct process*
process_new(pid_t pid, char *command, pid_t ppid)
{
	struct process *proc = malloc(sizeof(struct process));
	if (proc == NULL)
		return NULL;
	size_t cmdLen = strlen(command);
	proc->cmd = malloc(cmdLen + 1); /* extra for nul terminator */
	if (proc->cmd == NULL)
		return NULL;
	strncpy(proc->cmd, command, cmdLen);
	proc->pid = pid;
	proc->ppid = ppid;
	proc->firstChild = proc->lastChild = NULL;
	return proc;
}

struct pnode*
pnode_new(struct process *proc)
{
	struct pnode *node = malloc(sizeof(struct pnode));
	if (node == NULL)
		return NULL;
	node->proc = proc;
	node->left = node->right = NULL;
	return node;
}

struct pnode*
pnode_find(struct pnode *node, pid_t pid)
{
	if (node == NULL)
		return NULL;
	else if (pid < node->proc->pid)
		return pnode_find(node->left, pid);
	else if (pid > node->proc->pid)
		return pnode_find(node->right, pid);
	else
		return node;
}

struct pnode*
pnode_insert(struct pnode *node, struct process *proc)
{
	if (node == NULL) {
		node = pnode_new(proc);
	} else if (proc->pid < node->proc->pid) {
		node->left = pnode_insert(node->left, proc);
	} else if (proc->pid > node->proc->pid) {
		node->right = pnode_insert(node->right, proc);
	}
	return node;
}

struct proclist*
process_insert_child(struct process *parent, struct process *child)
{
	struct proclist *childnode = NULL;
	if (parent != NULL && child != NULL) {
		/* Allocate memory for node */
		childnode = malloc(sizeof(struct proclist));
		if (childnode == NULL)
			return NULL;
		childnode->next = NULL;
		childnode->proc = child;
		if (parent->firstChild == NULL) {
			parent->firstChild = parent->lastChild = childnode;
		} else {
			parent->lastChild->next = childnode;
			parent->lastChild = childnode;
		}
	}
	return childnode;
}
