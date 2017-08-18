/* freebufpool.c - freebufpool */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  freebufpool  -  Deallocates memory for a buffer pool
 *------------------------------------------------------------------------
 */
syscall	freebufpool(
	  bpid32	poolid		/* ID of pool to free		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	int32 totalbufs = 0;
	struct bpentry *bpptr;
	mask = disable();
	int count = bufsavail(poolid, &totalbufs);
	if(count < totalbufs){
		return SYSERR;
	} 
	bpptr = &buftab[poolid];
	char *buf = (char *)bpptr->bpnext;
	int32 bufsize = bpptr->bpsize;
	freemem(buf, totalbufs*(bufsize+sizeof(bpid32)));
	nbpools--;
	bpptr->bpnext = NULL; 

	restore(mask);			/* Restore interrupts		*/
	return OK;
}
