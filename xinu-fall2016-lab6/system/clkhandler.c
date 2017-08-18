/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	volatile struct hpet_csreg *csrptr;
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/

	csrptr = (struct hpet_csreg *)HPET_BASE_ADDR;

	/* Check the interrupt status */

	if(!(csrptr->gis & HPET_GIS_T0)) { /* No interrupt */
		return;
	}

	/* Acknowledge the interrupt by writing 1 in the Timer 0 bit */

	csrptr->gis |= HPET_GIS_T0;

	/* Decrement the ms counter, and see if a second has passed */

	if((--count1000) <= 0) {

		/* One second has passed, so increment seconds count */

		clktime++;

		/* Reset the local ms counter for the next second */

		count1000 = 1000;
	}

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--preempt) <= 0) {
		preempt = QUANTUM;
		resched();
	}
}
