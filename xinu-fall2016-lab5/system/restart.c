/* restart.c - restart */

#include <xinu.h>

/*
void restore_freelist(){

	struct memblk *cur = head;

	memlist.mnext = cur->mnext;
	memlist.mlength = cur->mlength;
	
	//struct memblk *cur = ((struct memblk *)memlist).mnext;
	//kprintf("%p..........\n", (void *)(head->mnext));
	//kprintf("%u..........\n\n\n", head->mlength);
	//kprintf("%p..........\n", (void *)(memlist.mnext));
	//kprintf("%u..........\n", memlist.mlength);
	
	cur = cur+1;
	struct memblk *cur1 = (struct memblk *)memlist.mnext;
	int i=1;
	
	while(i<snapshot_size){
		cur1->mnext = cur->mnext;
		cur1->mlength = cur->mlength;
		i++;
		//kprintf("%p\n", (void *)cur->mnext);
		cur++;
		//kprintf("%p\n", (void *)cur1->mnext);
		//kprintf("%u\n\n\n", cur1->mlength);
		cur1 = cur1->mnext;
	}
	cur1->mnext = NULL;
}
*/

void restart_call(uint32 delay){
	int i;
	struct procent *prptr;
	kprintf("restarting...currpid is %d....restart_flag\n", currpid);
	for(i=0;i<NPROC;i++){
		prptr = &proctab[i];
		if(prptr->prstate != PR_FREE && sys_proc[i] == FALSE && infotab[i].nargs != -1 && i!=currpid){
			kill(i);
			kprintf("%d...%d\n", i, infotab[i].nargs);
		}
	}

	//restore_freelist();
	
	sleep(delay);

	for(i=0;i<NPROC;i++){
		//kprintf("hellooooo....%d\n", i);
		if(sys_proc[i] == FALSE && infotab[i].nargs != -1 && i!=currpid && infotab[i].oldpid == -1){
			resume(recreate(infotab[i].funcaddr, infotab[i].ssize, infotab[i].priority, infotab[i].name, i));
			//kprintf("new pid for %d is %d\n", i, pid);
		}
	}
}


/*------------------------------------------------------------------------
 *  restart  -  Restarts user processes
 *------------------------------------------------------------------------
 */
syscall	restart(
	  uint32	delay		/* Time to delay in seconds	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();

	resume(create((void *)restart_call, INITSTK, INITPRIO,
					"Restart process", 1, delay));
	//kill
	//delay
	//restart user process
	restore(mask);			/* Restore interrupts		*/
	return OK;
}
