/*
 * Establish a real-time timer that expires once after the specified number of
 * seconds have passed, with no repeating interval.
 * 
 * After seconds have passed, SIGALRM is delivered to the calling process.
 * Overrides any previously set timer; if seconds is 0, an existing timer is disabled.
 *
 * Always succeeds, returning the number of seconds remaining on any previously
 * set timer, or 0 if no timer was previously set.
 */
unsigned int s_alarm(unsigned int seconds);
