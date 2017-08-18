/* hpetinit.c  -  hpetinit */

#include <xinu.h>

struct	hpetcblk hpettab[1];

/*------------------------------------------------------------------------
 * hpetinit  -  Initialize the HPET device
 *------------------------------------------------------------------------
 */
devcall	hpetinit (
		struct	dentry *devptr	/* Entry in device switch table	*/
		)
{
	struct	hpetcblk *hpetptr;	/* Pointer to control block	*/
	volatile struct hpet_csreg *csrptr;
					/* Pointer to HPET registers	*/

	hpetptr = &hpettab[devptr->dvminor];

	/* Get the address of control and status registers */
	csrptr = (struct hpet_csreg *)devptr->dvcsr;
	hpetptr->csr = devptr->dvcsr;

	/* Interrupt Type is 'level-triggered' */
	csrptr->t1cc_l = HPET_TXCC_IT;

	/* TODO: Set interrupt handler for HPET Timer1 device using
	 * the set_evec() function
	 */
	set_evec( devptr->dvirq, (uint32)devptr->dvintr );
	//initialize the data structure that you have defined
	hpetptr->hook_func = NULL;

	return OK;
}
