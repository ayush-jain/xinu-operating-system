/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

void myproccleanup(void)
{
	kprintf("CHILD CLEANINGUP\n");
}

void myproc(int a, int b, int c)
{
	kprintf("CHILD REGCLEANUP\n");
	regcleanup(myproccleanup);
	kprintf("CHILD RUNNING\n");
	regcleanup(NULL);
	kprintf("CHILD REMOVED CLEANUP\n");
	while(1);
}

process	main(void)
{
	pid32 childpid;
	
	kprintf("MAIN CREATE CHILD\n");
	childpid = create(myproc, 4096, 10, "TEST", 3, 1, 2, 3);
	resume(childpid);

	kprintf("MAIN SLEEP\n");
	sleepms(100);
	
	kprintf("MAIN KILL CHILD\n");
	kill(childpid);

	return OK;
}
