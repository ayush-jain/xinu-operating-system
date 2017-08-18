/* regcleanup.c - regcleanup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  regcleanup  -  Allows a process to register a cleanup function
 *------------------------------------------------------------------------
 */
syscall	regcleanup(
	  void*		func		/* Pointer to cleanup function	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct procent *pt;
	mask = disable();

	pt = &proctab[currpid];
	pt->proc_cleanup = func;
	//kprintf("registering cleanup function\n\n\n");

	restore(mask);			/* Restore interrupts		*/
	return OK;
}
