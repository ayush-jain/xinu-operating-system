/* userret.c - userret */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  userret  -  Called when a process returns from the top-level function
 *------------------------------------------------------------------------
 */
void	userret(void)
{
	pid32 pid = getpid();
	//kprintf("helooooo...\n");
	int32 nargs = infotab[pid].nargs;
	if(nargs != -1){
		freemem((char *)infotab[pid].list, nargs*sizeof(uint32));
	}
	infotab[pid].list = NULL;
	infotab[pid].nargs = -1;
	infotab[pid].oldpid = -1;
	//kprintf("nooooo\n");
	kill(getpid());			/* Force process to exit */
}
