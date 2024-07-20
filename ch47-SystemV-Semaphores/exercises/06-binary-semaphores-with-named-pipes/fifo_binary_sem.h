#ifndef _FIFO_BINARY_SEMAPHORE_H
#define _FIFO_BINARY_SEMAPHORE_H

struct sempipe {
	int readFd;
	int writeFd;
};

/*
 * Given a FIFO path and initializes it for use as a semaphore. The semaphore is available for reservation.
 * On success, returns its fd, or -1 on error
 */
int initSemAvailable(char *pathname, struct sempipe* sempipe_buf);

/* Closes the associated file descriptors. Assumes it was opened
   with initSemAvailable(), and that the files descriptors were not otherwise touched */
int closeSem(struct sempipe* sempipe_buf);

/* Given a FIFO pathname initialized via initSemAvailable() or initSemInUse(), reserves the
   semaphore, blocking until it is available. On success returns 0, or -1 on error. */
int reserveSem(char *pathname);

/* Given a FIFO pathname via initSemAvailable() or initSemInUse(), releases the
   semaphore. On success returns 0, or -1 on error. */
int releaseSem(char *pathname);

/* Given FIFO pathname initialized via initSemAvailable() or initSemInUse(), reserves the
   semaphore. On success returns 0. On error returns -1, and if the semaphore was in use, sets errno
   to EAGAIN */
int reserveSemNB(char *pathname);

#endif
