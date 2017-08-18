/* regevent.c - regevent */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  regevent  -  Register for an asynchronous event
 *------------------------------------------------------------------------
 */

syscall	regevent(
	  void (*event_handler)(uint32 event),	/* Event handler	*/
	  uint32 event				/* Event identifier	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();

	/* Place code for regevent here					*/
	if(event >= MAX_EVENTS || event <= 0){
		restore(mask);
		return SYSERR;
	}
	
	evtab[event][currpid].event_handler = event_handler; //the handler is automatically updated if it's not null 
	
	restore(mask);			/* Restore interrupts		*/
	return OK;
}