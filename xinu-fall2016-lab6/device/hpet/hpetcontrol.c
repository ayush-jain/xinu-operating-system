/* hpetcontrol.c  -  hpetcontrol */

#include <xinu.h>

/*------------------------------------------------------------------------
 * hpetcontrol  -  Control function for the hpet device
 *------------------------------------------------------------------------
 */
devcall	hpetcontrol (
		struct	dentry *devptr,	/* Entry in device switch table	*/
		int32	func,		/* Function to perform		*/
		int32	arg1,		/* Argument 1 for request	*/
		int32	arg2		/* Argument 2 for request	*/
		)
{
	struct	hpetcblk *hpetptr;	/* Pointer to control block	*/
	volatile struct hpet_csreg *csrptr;
					/* Control and status registers	*/
	int32	tti;			/* Time to interrupt		*/
	uint32 	val;
	uint32 ticks_1ms;

	/* Get pointer to control block */

	hpetptr = &hpettab[devptr->dvminor];

	/* Get pointer to control and status registers */

	csrptr = (struct hpet_csreg *)hpetptr->csr;

	switch(func) {

	/* TODO: Set interrupt for specified time in the future
	 *
	 * arg1 provides the time in milliseconds
	 *
	 * arg2 must be ignored
	 *
	 */
	case HPET_CTRL_INTR_SET:
		val = csrptr->mcv_l;
		ticks_1ms = 14318; /* ticks_1ms = 1000000/69.841279 */
		csrptr->t1cv = (val+ (ticks_1ms*arg1)); //this might overflow, works till around 150s
		//kprintf("computed counter val is %u...\n", csrptr->t1cv); 
		csrptr->t1cc_l |= (1 << 2);
		break;	

	/* TODO: Disable interrupt */
	case HPET_CTRL_INTR_DEL:
		csrptr->t1cc_l &= ~(1 << 2);
		break;

	/* Already implemented. DO NOT modify
	 *
	 * arg1 provides the address of location where time_to_interrupt
	 * needs to be written (-1 is written if Timer1 interrupt is not
	 * enabled)
	 *
	 * arg2 is ignored
	 *
	 */
	case HPET_CTRL_TTI:
		if(!(csrptr->t1cc_l & HPET_TXCC_IE)) {
			*((int32 *)arg1) = -1;
		}
		else {
			tti = (int32)(csrptr->t1cv - csrptr->mcv_l);
			tti = tti/14318;
			*((int32 *)arg1) = tti;
		}
		break;

	/* TODO: Register a hook function for Timer1 interrupt
	 *
	 * arg1 provides the function pointer to the hook function
	 *
	 * arg2 is ignored
	 *
	 */
	case HPET_CTRL_HOOK_SET:
		hpetptr->hook_func = (void (*)(void))arg1;		
		break;
	}

	return OK;
}
