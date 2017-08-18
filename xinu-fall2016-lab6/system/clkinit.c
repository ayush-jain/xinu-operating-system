/* clkinit.c - clkinit (x86) */

#include <xinu.h>

uint32	clktime;		/* Seconds since boot			*/
uint32	ctr1000 = 0;		/* Milliseconds since boot		*/
qid16	sleepq;			/* Queue of sleeping processes		*/
uint32	preempt;		/* Preemption counter			*/

/*------------------------------------------------------------------------
 * clkinit  -  Initialize the clock and sleep queue at startup (x86)
 *------------------------------------------------------------------------
 */
void	clkinit(void)
{
	volatile struct	hpet_csreg *csrptr;

	/* Allocate a queue to hold the delta list of sleeping processes*/

	sleepq = newqueue();

	/* Initialize the preemption count */

	preempt = QUANTUM;

	/* Initialize the time since boot to zero */

	clktime = 0;

	/* Set interrupt vector for the clock to invoke clkdisp */

	set_evec(IRQBASE, (uint32)clkdisp);

	csrptr = (struct hpet_csreg *)HPET_BASE_ADDR;

	/* Disable the HPET first */

	csrptr->gc = 0;

	/* Set Legacy Routing for HPET */

	csrptr->gc |= HPET_GC_LRE;

	/* Initialize the Timer 0 for periodic interrupts */

	csrptr->t0cc_l = HPET_TXCC_IT |	/* Level-triggered interrupt 	*/
		         HPET_TXCC_IE |	/* Interrupt enabled		*/
			 0x00000008;	/* Periodic mode		*/

	/* Set the comparator value for Timer 0 such that	*/
	/* it triggers interrupt every 1 ms			*/

	csrptr->t0cc_l |= 0x00000040;
	csrptr->t0cv_l = 14318;
	csrptr->t0cc_l |= 0x00000040;
	csrptr->t0cv_u = 0;

	/* Set the main counter to zero */

	csrptr->mcv_l = 0;
	csrptr->mcv_u = 0;

	/* Enable the HPET now */

	csrptr->gc |= HPET_GC_OE;

	return;
}
