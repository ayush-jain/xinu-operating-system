/* alarm_trigger.c - alarm_trigger */

#include <xinu.h>


/*------------------------------------------------------------------------
 * alarm_trigger  -  This function gets called by HPET interrupt handler
 *------------------------------------------------------------------------
 */
void	alarm_trigger (void) {

	/* TODO: Fill in your code here
	 *
	 * Upon registration with HPET device, this function will be
	 * called whenever there is an HPET interrupt.
	 *
	 * You must trigger alarm(s) (there could be multiple alarms
	 * that need to be triggered) as a part of this function.
	 *
	 * NOTE: This function is called at interrupt time when interrupts
	 * are disabled. DO NOT call functions like sleep, wait, receive
	 * that will make the current process ineligible to execute.
	 *
	 */
	int32 aid = alarmtab[alarmqhead(alarmqid)].anext;
	int32 prev;
	int32 next;

	if(alarmqsize > 0){
		infotab[aid].callbackfn(infotab[aid].arg);
		int status = remove_alarm(aid, alarmqid);
		if(status == OK){
			//kprintf("Removed Successfully....\n");
			infotab[aid].arg = -1;
			infotab[aid].callbackfn = NULL;
		}
	}

	prev = alarmqhead(alarmqid);
	next = alarmtab[alarmqhead(alarmqid)].anext;

	while ((next != alarmqtail(alarmqid)) && (alarmtab[next].akey == 0)) {
		infotab[next].callbackfn(infotab[next].arg);
		infotab[next].arg = -1;
		infotab[next].callbackfn = NULL;
		next = alarmtab[next].anext;
		alarmqsize--;
	}
	alarmtab[prev].anext = next;
	alarmtab[next].aprev = prev;

	if(alarmqsize > 0){
		control(HPET, HPET_CTRL_INTR_SET, alarmtab[next].akey, 0);
	}

}
