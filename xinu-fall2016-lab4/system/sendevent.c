/* sendevent.c - sendevent */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  sendevent  -  Send an event to a process
 *------------------------------------------------------------------------
 */
syscall	sendevent(
	  pid32 pid,			/* ID of recipient process	*/
	  uint32 event			/* Event identifier		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	/* Place code for sendevent here				*/
	if(event < USER_EVENT_BOUNDARY || event >= MAX_EVENTS){
		restore(mask);
		return SYSERR;
	}
	if (evtab[event][pid].event_handler == NULL) {
		restore(mask);
		return SYSERR;
	} else if(evtab[event][pid].trigger_flag == 1) {
		restore(mask);
		return SYSERR;
	} else {
		evtab[event][pid].trigger_flag = 1;
	}

	if(currpid == pid){
		evtab[event][currpid].event_handler(event);
		evtab[event][currpid].trigger_flag = 0;
	}

	restore(mask);			/* Restore interrupts		*/
	return OK;
}

syscall	sendreservedevent(
	  pid32 pid,			/* ID of recipient process	*/
	  uint32 event			/* Event identifier		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	/* Place code for sendevent here	
	//handles te cases of all user events as well as event no greater than max_events */
	if(event >= USER_EVENT_BOUNDARY || event <= 0){
		restore(mask);
		return SYSERR;
	}
	if (evtab[event][pid].event_handler == NULL) {
		restore(mask);
		return SYSERR;
	} else if(evtab[event][pid].trigger_flag == 1) {
		restore(mask);
		return SYSERR;
	} else {
		evtab[event][pid].trigger_flag = 1;
	}

	if(currpid == pid){
		evtab[event][currpid].event_handler(event);
		evtab[event][currpid].trigger_flag = 0;
	}

	restore(mask);			/* Restore interrupts		*/
	return OK;
}

/*------------------------------------------------------------------------
 *  broadcastevent  -  Send an event to all processes for that particular event 
 *------------------------------------------------------------------------
 */
syscall	broadcastevent(
	 uint32 event			/* Event identifier		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	pid32 i;

	mask = disable();

	if(event < USER_EVENT_BOUNDARY || event >= MAX_EVENTS){
		restore(mask);
		return SYSERR;
	}
	for(i=1;i<NPROC+1;i++){
		if (evtab[event][i].event_handler == NULL) {
			continue;
		} else if(evtab[event][i].trigger_flag == 1) {
			continue;
		} else {
			evtab[event][i].trigger_flag = 1;
		}

		if(currpid == i){
			evtab[event][currpid].event_handler(event);
			evtab[event][currpid].trigger_flag = 0;
		} 
	} 
	
	restore(mask);			/* Restore interrupts		*/
	return OK;
}
