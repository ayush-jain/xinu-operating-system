/*  main.c  - main */

#include <xinu.h>

#define ITERATIONCOUNT 10
#define BPCOUNT 5

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
	uint32 i;
	uint32 j;
	bpid32 pool[BPCOUNT];
	char* bufptr;

	kprintf("INITIAL FREELIST\n");
	dumpfreelist();

	for(j = 0; j < ITERATIONCOUNT; j++) {
	
		for(i = 0; i < BPCOUNT; i++) {
			pool[i] = mkbufpool(10*(i+1), 100*(i+1));
		}
		
		kprintf("FREELIST AFTER MKBUFPOOL\n");
		dumpfreelist();

		bufptr = getbuf(pool[0]);
		kprintf("ALLOCATED BUFFER %08X\n", (uint32)bufptr);
		kprintf("FREEBUFF RC: %d\n", freebuf(bufptr));

		for(i = 0; i < BPCOUNT; i++) {
			kprintf("FREEBUFF POOL RC: %d\n", freebufpool(pool[i]));
		}
		
		kprintf("FREELIST AFTER FREEBUFPOOL\n");
		dumpfreelist();
	}
	
	return OK;
}
