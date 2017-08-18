/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	
	//int i;
	/* Run the Xinu shell */

	//recvclr();
	//resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	

	int myvalue = 2;
	
	//* Load the library 
	if(load_library("myadd") == SYSERR) {
		return;
	}
	
	

	//* Find the add1 function 
	int32 (*add1)(int32) = find_library_function("add1");
	if((int32)add1 == SYSERR) {
		kprintf("error..1\n");
		return -1;
	}

	int32 (*add2)(int32) = find_library_function("add2");
	if((int32)add2 == SYSERR) {
		kprintf("error.2..\n");
		return -1;
	}
	
	
	//
	kprintf("%d ooooo\n", add1(myvalue));
	kprintf("%d ooooo\n", add2(myvalue));
		
	/** TESTCASE 1 
	void *helloworld = load_program("helloworld");
	
		
	kprintf("\ncreating process..\n");
	pid32 pid = (create(helloworld, 4096, 20, "helloworld", 0, NULL));
	kprintf("pid is %d\n", pid);
	int prio = resume(pid);
	kprintf("prio is %d\n", prio);
	
	**/
	
	/** TESTCASE 2
	void *helloworld = load_program("ls");
	
		
	kprintf("\ncreating process..\n");
	pid32 pid = (create(helloworld, 4096, 20, "helloworld", 0, NULL));
	kprintf("pid is %d\n", pid);
	int prio = resume(pid);
	kprintf("prio is %d\n", prio);
	
	**/

	/** TESTCASE 3
	void *helloworld = load_program("semdump");
	
		
	kprintf("\ncreating process..\n");
	pid32 pid = (create(helloworld, 4096, 20, "helloworld", 0, NULL));
	kprintf("pid is %d\n", pid);
	int prio = resume(pid);
	kprintf("prio is %d\n", prio);
	
	**/

	/*
	while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}
	
	*/
	
	return OK;
}
