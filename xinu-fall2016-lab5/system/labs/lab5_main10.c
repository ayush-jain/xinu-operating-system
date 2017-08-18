/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

int32 ta_tc_restart_flag = 0;

void myproccleanup(void)
{
	kprintf("CLEANINGUP\n");
}

void myproc(int a, int b, int c)
{
	kprintf("%d %d %d REGCLEANUP\n", a, b, c);
	regcleanup(myproccleanup);
	kprintf("%d %d %d RUNNING\n", a, b, c);
	while(1) { 
		sleep(1); 
	}
}

process	main(void)
{
	kprintf("MAIN STARTED\n");
	resume(create(myproc, 4096, ta_tc_restart_flag+5, "TEST", 3, 20, 30, ta_tc_restart_flag+1));

	if(ta_tc_restart_flag == 0) {
		kprintf("BEFORE RESTART\n");
		ta_tc_restart_flag = 1;
		restart(1);
	}
	kprintf("MAIN AFTER RESTART\n");

	return OK;
}
