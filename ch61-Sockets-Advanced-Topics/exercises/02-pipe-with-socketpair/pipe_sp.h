#ifndef _PIPE_SOCKETPAIR_H
#define _PIPE_SOCKETPAIR_H

/* Creates a pipe and places two open file descriptors on the argument buffer. The read end
   is filedes[0], and the write end is filedes[1]. Returns 0 ons uccess, and -1 on failure.
*/
int pipe_sp(int filedes[2]);

#endif
