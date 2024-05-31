#include <pthread.h> /* pthread_mutex_t */

typedef struct {
	int initRan;
	pthread_mutex_t mtx;
} oticontrol_s;


/*
 * Runs the init function if it has not run yet in a previous call to this function
 * with the given control argument. Expects control to be statically allocated.
 * 
 * On success, returns 0, otherwise it returns positive error number
 */
int one_time_init(oticontrol_s *control, void (*init)(void));
