/* alarm_init.c - alarm_init */

#include <xinu.h>

struct aentry	alarmtab[NAENT];
struct alarm_info infotab[NALARMS];
int alarmqid = -1;
int alarmqsize;

int32	getalarmitem(
	  int32		aid		/* ID of alarm to remove	*/
	);

qid16	newalarm_queue(void)
{
	static qid16	nextqid=NALARMS;	/* Next list in alarmtab to use	*/
	qid16		q;		/* ID of allocated queue 	*/

	q = nextqid;
	if (q >= NAENT) {		/* Check for table overflow	*/
		return SYSERR;
	}

	/* Initialize head and tail nodes to form an empty queue */

	alarmtab[alarmqhead(q)].anext = alarmqtail(q);
	alarmtab[alarmqhead(q)].aprev = EMPTY;
	alarmtab[alarmqhead(q)].akey  = 0;
	alarmtab[alarmqtail(q)].anext = EMPTY;
	alarmtab[alarmqtail(q)].aprev = alarmqhead(q);
	alarmtab[alarmqtail(q)].akey  = MAXKEY;
	return q;
}



/*------------------------------------------------------------------------
 * alarm_init  -  Initializes the Xinu Alarm System
 *------------------------------------------------------------------------
 */
void	alarm_init (void) {

	intmask	mask;	/* Saved interrupt mask	*/

	mask = disable();

	/* TODO:
	 *
	 * 1. Initialize your data structures here
	 *
	 * 2. Register a hook function - alarm_trigger with the HPET
	 *
	 * Note: This function is already called as part of startup
	 * process, DO NOT explicitly call this function in your code.
	 *
	 */
	alarmqid = newalarm_queue();
	alarmqsize = 0;
	int i;
	for(i=0;i<NALARMS;i++){
	 	infotab[i].callbackfn = NULL;
	 	infotab[i].arg = -1;
	}

	control(HPET, HPET_CTRL_HOOK_SET, (int)alarm_trigger, 0);

	control(HPET, HPET_CTRL_INTR_DEL, 0, 0);

	restore(mask);
}


int32	getfirstalarm(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove an alarm (assumed	*/
					/*   valid with no check)	*/
{
	int32	head;

	if (isempty_aq(q)) {
		return EMPTY;
	}

	head = alarmqhead(q);
	return getalarmitem(alarmtab[head].anext);
}


int32	getlastalarm(
	  qid16		q		/* ID of queue from which to	*/
	)				/* Remove an alarm (assumed	*/
					/*   valid with no check)	*/
{
	int32 tail;

	if (isempty_aq(q)) {
		return EMPTY;
	}

	tail = alarmqtail(q);
	return getalarmitem(alarmtab[tail].aprev);
}


int32	getalarmitem(
	  int32		aid		/* ID of alarm to remove	*/
	)
{
	int32	prev, next;

	next = alarmtab[aid].anext;	/* Following node in list	*/
	prev = alarmtab[aid].aprev;	/* Previous node in list	*/
	alarmtab[prev].anext = next;
	alarmtab[next].aprev = prev;

	alarmqsize--;
	return aid;
}

status	insert_alarm(			/* Assumes interrupts disabled	*/
	  int32		aid,		/* ID of alarm to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  uint32	key		/* Delay from "now" (in ms.)	*/
	)
{
	int32	next;			/* Runs through the alarm list	*/
	int32	prev;			/* Follows next through the list*/

	if (isbadaid(aid)) {
		return SYSERR;
	}

	prev = alarmqhead(q);
	next = alarmtab[alarmqhead(q)].anext;
	
	int32 tti;
	control(HPET, HPET_CTRL_TTI, (int32)&tti, 0);	
	
	
	if(next != alarmqtail(q) && tti != -1){
		//kprintf("tti for aid %d is %d\n", aid, tti);
		alarmtab[next].akey = (uint32)tti;	
	}
	
	while ((next != alarmqtail(q)) && (alarmtab[next].akey <= key)) {
		key -= alarmtab[next].akey;
		prev = next;
		next = alarmtab[next].anext;
	}

	/* Insert new node between prev and next nodes */

	alarmtab[aid].anext = next;
	alarmtab[aid].aprev = prev;
	alarmtab[aid].akey = key;
	alarmtab[prev].anext = aid;
	alarmtab[next].aprev = aid;
	if (next != alarmqtail(q)) {
		alarmtab[next].akey -= key;
	}

	alarmqsize++;
	if(aid == alarmtab[alarmqhead(q)].anext){
		control(HPET, HPET_CTRL_INTR_SET, alarmtab[aid].akey, 0);	
	}

	return OK;
}

status	remove_alarm(
	  int32		aid,		/* ID of alarm to remove	*/
      qid16		q)
{
    int32	aidnext;		/* ID of alarm on alarm queue	*/
					/*   that follows the alarm	*/
					/*   which is being removed	*/

	bool8 first = FALSE;
	if (isbadaid(aid) || ((infotab[aid].arg == -1) && (infotab[aid].callbackfn == NULL))) {
		return SYSERR;
	}
	int32 tti;
	control(HPET, HPET_CTRL_TTI, (int32)&tti, 0);	
	
	int32 next = alarmtab[alarmqhead(q)].anext;
	if(next != alarmqtail(q) && tti != -1){
		//kprintf("tti while removal for aid %d is %d\n", aid, tti);
		alarmtab[next].akey = (uint32)tti;	
	} else if(next != alarmqtail(q) && tti == -1){
		alarmtab[next].akey = 0;
	}

	/* Increment delay of next alarm */

	aidnext = alarmtab[aid].anext;
	if (aidnext < NALARMS) {
		alarmtab[aidnext].akey += alarmtab[aid].akey;
	}
	if(aid == alarmtab[alarmqhead(q)].anext){
		first = TRUE;	
	}
	getalarmitem(aid);
	if(alarmqsize == 0){
		//kprintf("disabled...as size is 0\n");
		control(HPET, HPET_CTRL_INTR_DEL, 0, 0);
	} else if(first == TRUE && alarmtab[next].akey > 0){
		//kprintf("magic...\n");
		control(HPET, HPET_CTRL_INTR_SET, alarmtab[next].akey, 0);
	}			
	return OK;
}
