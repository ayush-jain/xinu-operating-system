#include <xinu.h>

int main(){

	//iterate over NSEM entries in semtab to see which entries are free or used state
	int i;
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/
	char *str;
	char *sem_free = "S_FREE";
	char *sem_used = "S_USED"; 

	kprintf("%8s %s %8s %8s\n", "Entry", "State", "Count", "Queue");

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		if(semptr->sstate == S_FREE){
			str = sem_free;
		} else if(semptr->sstate == S_USED){
			str = sem_used;
		}
		kprintf("%8d %-6s %8d %8d\n", i, str, semptr->scount, semptr->squeue);		
	}

	return OK;
}

