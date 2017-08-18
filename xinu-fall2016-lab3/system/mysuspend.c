/* mysuspend.c - mysuspend */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  mysuspend  -  Suspend a process (even if it is not in a suspendable
 *                state, placing it in hibernation or delayed hibernation
 *------------------------------------------------------------------------
 */
syscall	mysuspend(
	  pid32		pid		/* ID of process to suspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	if ((prptr->prstate == PR_SUSP) || (prptr->delayed_suspend_flag == TRUE)) {
		restore(mask);
		return SYSERR;
	}
	if (prptr->prstate == PR_READY) {
		getitem(pid);		    /* Remove a ready process	*/
					    /*   from the ready list	*/
		prptr->prstate = PR_SUSP;
	} else if (prptr->prstate == PR_CURR) {
		prptr->prstate = PR_SUSP;   /* Mark the current process	*/
		resched();		    /*   suspended and resched.	*/
	} else if (prptr->prstate == PR_WAIT || prptr->prstate == PR_RECV || prptr->prstate == PR_SLEEP || prptr->prstate == PR_RECTIM){
		prptr->delayed_suspend_flag = TRUE;	
	}
	restore(mask);
	return OK;
}
