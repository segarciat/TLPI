#include <fcntl.h>      /* Defines O_* constants use for oflag parameter */
#include <sys/stat.h>   /* Defines mode constants used for permissions */
#include <semaphore.h>


typedef struct {
    int semid;
	int fd;
} psem_t;

#define PSEM_FAILED ((psem_t *) 0)

/* 
  Open an existing semaphore, or create a new semaphore.
  Returns pointer to semaphore on success, or SEM_fAILED on error.
*/
psem_t* psem_open(const char *name, int oflag, mode_t mode, unsigned int value);

/* 
  Close a currently opened semaphore. Returns 0 on success, and -1 on error.
 */
int psem_close(psem_t *sem);

/* 
  Remove an semaphore identified by the name argument. The semaphore is marked
  to be destroyed once all processes have ceased using it. Returns 0 on success, -1 on error.
 */
int psem_unlink(const char *name);

/* 
  Decrement semaphore value by 1. If value is greater than 0, return immediately.
  Otherwise, block until its value is greater than 0.
  Returns 0 on success, -1 on error.
 */
int psem_wait(psem_t *sem);

/* 
  Like sem_wait(), but if the attempt would block, it fails immediately with errno EAGAIN and
  returns -1. Returns 0 on success, and -1 on error.
 */
int psem_trywait(psem_t *sem);

/* 
  Like sem_wait(), but blocks until the specified time, and if the timeout expires,
  fails with ETIMEDOUT. Returns 0 on success, and -1 on error.
 */
int psem_timedwait(psem_t *sem, const struct timespec *abs_timeout);

/* Increment semaphore value by 1 and returns. Returns 0 on success and -1 on error. */
int psem_post(psem_t *sem);

/* Get the current value of the semaphore and return its value in the sval buffer.
  Returns 0 on success and -1 on error.
 */
int psem_getvalue(psem_t *sem, int *sval);
