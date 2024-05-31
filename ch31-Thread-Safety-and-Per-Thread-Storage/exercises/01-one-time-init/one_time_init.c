#include "one_time_init.h"

int
one_time_init(oticontrol_s *control, void (*init)(void))
{
	int s;
	/* Lock mutex */
	if ((s = pthread_mutex_lock(&control->mtx)) != 0)
		return s;
	
	/* Run if necessart */
	if (!control->initRan)
		init();
	control->initRan = 1;

	/* Release lock */
	s = pthread_mutex_unlock(&control->mtx);
	return s;
}
