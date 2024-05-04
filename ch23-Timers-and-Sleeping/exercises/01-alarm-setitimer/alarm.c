#include <sys/time.h>
#include "alarm.h"

unsigned int s_alarm(unsigned int seconds)
{
	struct itimerval newitv, olditv;

	/* Expiration time */
	newitv.it_value.tv_sec = seconds;
	newitv.it_value.tv_usec = 0;

	/* No interval */
	newitv.it_interval.tv_sec = 0;
	newitv.it_interval.tv_usec = 0;

	/* Establish the timer, generating SIGALRM when it expires */
	setitimer(ITIMER_REAL, &newitv, &olditv);

	/* Restore previous timer remainder, or 0 if none */
	return olditv.it_value.tv_sec;
}
