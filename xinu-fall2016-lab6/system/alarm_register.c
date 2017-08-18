/* alarm_register.c - alarm_register */

#include <xinu.h>

local	int32 newaid();

/*------------------------------------------------------------------------
 * alarm_register  -  Regiater an alarm with the Alarm System
 *------------------------------------------------------------------------
 */
int32	alarm_register (
		int32	delay,			/* Time in milliseconds	*/
		void	(*callbackfn)(int32),	/* Callback fn pointer	*/
		int32	cbarg			/* Callback fn argument	*/
		)
{
	/* TODO: Fill in your code here.
	 *
	 * Follow instructions from the lab description
	 *
	 */
	intmask	mask;	/* Saved interrupt mask	*/

	mask = disable(); 
	if(alarmqsize >= NALARMS){
		//kprintf("alarmqsize is %d\n", alarmqsize);
		restore(mask);
		return SYSERR;
	} 

	int32 aid = newaid();
	//kprintf("New aid is %d\n", aid);
	int status = insert_alarm(aid, alarmqid, delay);
	if(status == OK){
		//kprintf("Alarm inserted successfully\n");
		infotab[aid].callbackfn = callbackfn;
		infotab[aid].arg = cbarg;
	} else {
		restore(mask);
		return status;
	}
	restore(mask);
	return aid;

}

int32   alarm_deregister(
        int32 alarm_id
        ){
	intmask	mask;	/* Saved interrupt mask	*/

	mask = disable();
	
	int status = remove_alarm(alarm_id, alarmqid);
	if(status == OK){
		//kprintf("Deregistered successfully..\n");		
		infotab[alarm_id].arg = -1;
		infotab[alarm_id].callbackfn = NULL;
	} else {
		restore(mask);
		return SYSERR;
	}

	restore(mask);
	return OK; 
}

local	int32	newaid(void)
{
	uint32	i;			/* Iterate through all alarms*/
	static	int32 nextaid = 0;	/* Position in table to try or	*/
					/*   one beyond end of table	*/

	/* Check all NPROC slots */

	for (i = 0; i < NALARMS; i++) {
		nextaid %= NALARMS;	/* Wrap around to beginning */
		if (infotab[nextaid].arg == -1 && infotab[nextaid].callbackfn == NULL) {
			return nextaid++;
		} else {
			nextaid++;
		}
	}
	return (int32) SYSERR;
}
