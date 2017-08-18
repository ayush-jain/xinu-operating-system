/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

int32 ta_tc_restart_flag = 0;

process	main(void)
{
	kprintf("MAIN STARTED\n");
	if(ta_tc_restart_flag == 0) {
		kprintf("BEFORE RESTART\n");
		ta_tc_restart_flag = 1;
		restart(0);
	}
	kprintf("MAIN AFTER RESTART\n");

	return OK;
}
