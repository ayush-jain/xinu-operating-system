/*  main.c  - main */

#include <xinu.h>

#define BUFFERCOUNT 10

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
	int32 i;
	bpid32 pool;
	char* bufptr[BUFFERCOUNT];
	int32 totalbufs = 0;
	int32 availbufs = 0;

	kprintf("INITIAL FREELIST\n");
	dumpfreelist();

	pool = mkbufpool(10, 100);
	if(pool == SYSERR) {
		kprintf("UNABLE TO CREATE BUFFER POOL\n");
		return -1;
	}

	if(bufsavail(100, &totalbufs) != SYSERR) {
		kprintf("RETRIEVED %d TOTAL BUFFERS FROM INVALID POOL\n", totalbufs);
		return -1;
	}
	
	kprintf("FREELIST AFTER MKBUFPOOL\n");
	dumpfreelist();

	availbufs = bufsavail(pool, &totalbufs);
	kprintf("AVAIL: %d TOTAL: %d\n", availbufs, totalbufs);

	for(i = 0; i < BUFFERCOUNT; i++) {
		bufptr[i] = getbuf(pool);
		availbufs = bufsavail(pool, &totalbufs);
		kprintf("AVAIL: %d TOTAL: %d\n", availbufs, totalbufs);
	}
	
	for(i = 0; i < BUFFERCOUNT; i++) {
		freebuf(bufptr[i]);
		availbufs = bufsavail(pool, &totalbufs);
		kprintf("AVAIL: %d TOTAL: %d\n", availbufs, totalbufs);
	}

	if(freebufpool(pool) == SYSERR) {
		kprintf("UNABLE TO FREE BUFFER POOL\n");
	}

	kprintf("FREELIST AFTER FREEBUFPOOL\n");
	dumpfreelist();

	return OK;
}
