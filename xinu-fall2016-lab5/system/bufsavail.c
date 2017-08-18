/* bufsavail.c - bufsavail */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  bufsavail  -  Allows a process to register a cleanup function
 *------------------------------------------------------------------------
 */
int32	bufsavail(
	  bpid32	poolid,		/* ID of the buffer pool	*/
	  int32*	totalbufs	/* Total buffers in the pool	*/
	)
{
	intmask mask;
	struct bpentry *bpptr;

	mask = disable();
	if ( (poolid< 0 || poolid >= nbpools)) {
		restore(mask);
		return SYSERR;
	}

	bpptr = &buftab[poolid];
	int32 count = semcount(bpptr->bpsem);
	*totalbufs = bpptr->bufcount; 

	return count;
}
