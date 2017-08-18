/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

void dumpfreelist()
{
	struct	memblk	*memptr;	/* Ptr to memory block		*/

	for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
	    kprintf("[0x%08X to 0x%08X]\n",
		(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
	}
	kprintf("\n");
}

process	main(void)
{
	bpid32 pool;

	kprintf("INITIAL FREELIST\n");
	dumpfreelist();

	pool = mkbufpool(10, 100);
	if(pool == SYSERR) {
		kprintf("UNABLE TO CREATE BUFFER POOL\n");
		return -1;
	}
	
	kprintf("FREELIST AFTER MKBUFPOOL\n");
	dumpfreelist();

	if(freebufpool(100) != SYSERR) {
		kprintf("FREED INVALID BUFFER POOL\n");
		return -1;
	}

	if(freebufpool(pool) == SYSERR) {
		kprintf("UNABLE TO FREE BUFFER POOL\n");
	}

	kprintf("FREELIST AFTER FREEBUFPOOL\n");
	dumpfreelist();

	return OK;
}
