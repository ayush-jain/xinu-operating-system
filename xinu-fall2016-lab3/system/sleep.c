/* sleep.c - sleep sleepms */

#include <xinu.h>

#define	MAXSECONDS	2147483		/* Max seconds per 32-bit msec	*/

/*------------------------------------------------------------------------
 *  sleep  -  Delay the calling process n seconds
 *------------------------------------------------------------------------
 */
syscall	sleep(
	  int32	delay		/* Time to delay in seconds	*/
	)
{
	if ( (delay < 0) || (delay > MAXSECONDS) ) {
		return SYSERR;
	}
	sleepms(1000*delay);
	return OK;
}

/*------------------------------------------------------------------------
 *  sleepms  -  Delay the calling process n milliseconds
 *------------------------------------------------------------------------
 */
syscall	sleepms(
	  int32	delay			/* Time to delay in msec.	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/

	if (delay < 0) {
		return SYSERR;
	}

	if (delay == 0) {
		yield();
		return OK;
	}

	/* Delay calling process */

	mask = disable();
	if (insertd(currpid, sleepq, delay) == SYSERR) {
		restore(mask);
		return SYSERR;
	}

	proctab[currpid].prstate = PR_SLEEP;
	resched();
	restore(mask);
	return OK;
}

syscall	mysleepms(pid32 pid, int32 delay){
	intmask	mask;			/* Saved interrupt mask		*/
	register struct procent *prptr;

	if (delay < 0 || (delay > (MAXSECONDS*1000))) {
		return SYSERR;
	}

	/* Delay calling process */

	mask = disable();
	prptr = &proctab[pid];

	if (isbadpid(pid) || (pid == NULLPROC)) {
		restore(mask);
		return SYSERR;
	}

	if (prptr->prstate == PR_SLEEP) {
		restore(mask);
		return SYSERR;
	}

	if (prptr->prstate == PR_CURR) {
		restore(mask);
		sleepms(delay);
		return OK;
	}

	if (delay == 0 && prptr->my_delay <= 0) {
		prptr->my_delay = -1;
		restore(mask);
		return OK;
	}

	if(prptr->prstate == PR_READY) {
		if(prptr->my_delay < 0) {
			getitem(pid);
		}	
		prptr->my_delay = -1;
		if (insertd(pid, sleepq, delay) == SYSERR) {
			restore(mask);
			return SYSERR;
		}
		proctab[pid].prstate = PR_SLEEP;
		restore(mask);
		return OK;			
	}
	
	/*execution for the remaining states except CURR and READY */
	if(prptr->my_delay == -1){
		prptr->my_delay = delay;
	} else {
		restore(mask);
		return SYSERR;	// case when delayed sleep has already been set by some process
	}
	
	restore(mask);
	return OK;	
}
