/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	prptr = &proctab[pid];
	
	if(prptr->delayed_suspend_flag == TRUE){	
		prptr->prstate = PR_SUSP;
		prptr->delayed_suspend_flag = FALSE;	
		return SYSERR; // as ready was not successful in putting this process to ready state	
	}

	if(prptr->my_delay >=0) {
		prptr->prstate = PR_READY;	
		mysleepms(pid, prptr->my_delay);
		return SYSERR; // as ready was not successful in putting this process to ready state
	}

	/* Normal Execution- Set process state to indicate ready and add to ready list */

	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	resched();
	return OK;
}
