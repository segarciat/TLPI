#include <sys/types.h> /* pid_t, size_t */

struct process {
	pid_t pid;
	pid_t ppid;
	char *cmd;
	struct proclist *firstChild, *lastChild;
};

struct proclist {
	struct process *proc;
	struct proclist *next;
};

struct pnode {
	size_t index;
	struct process *proc;
	struct pnode *left, *right;
};

struct process *process_new(pid_t pid, char *command, pid_t ppid);
struct pnode *pnode_new(struct process *proc);
struct pnode *pnode_find(struct pnode *node, pid_t pid);
struct pnode *pnode_insert(struct pnode *node, struct process *proc);
struct proclist *process_insert_child(struct process *parent, struct process *proc);
