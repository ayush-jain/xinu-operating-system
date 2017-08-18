/* hpethandler.c - hpethandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * hpethandler  -  Interrupt handler for the HPET Timer 1
 *------------------------------------------------------------------------
 */
void	hpethandler (void) {

	struct	dentry *devptr;		/* Entry in device switch table	*/
	struct	hpetcblk *hpetptr;	/* Pointer to control block	*/
	volatile struct	hpet_csreg *csrptr;/* Address of HPET registers	*/

	devptr = &devtab[HPET];
	hpetptr = &hpettab[devptr->dvminor];
	csrptr = (struct hpet_csreg *)hpetptr->csr;


	/* TODO:
	 * 
	 * 1. Acknowledge the Timer1 interrupt
	 *
	 * 2. Disable Timer1 interrupt
	 *
	 * 3. Call the hook function if registered
	 *
	 */
	 csrptr->gis |= (1 << 1);
	 csrptr->t1cc_l &= ~(1 << 2);
	 
	 if(hpetptr->hook_func != NULL){
		hpetptr->hook_func();
	 }
	 		
}
