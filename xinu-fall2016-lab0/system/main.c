/*  main.c  - main */

#include <xinu.h>

void myprocess(int a) {
        kprintf("Hello world %d\n", a);
}

void myhungryprocess(void) {
        kprintf("I'm a looping process\n");
        while(TRUE) {
                // Do nothing forever
        }
}

process	main(void)
{
	
	/* Obtain an IP address */

	netstart();

	/* Run the Xinu shell */

	recvclr();
	//resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	resume(create(myhungryprocess, 4096, 1000, "hungryprocess", 0));
	resume(create(myprocess, 4096, 1000, "helloworld1", 1, 1));
	resume(create(myprocess, 4096, 50, "helloworld2", 1, 2));
	resume(create(myprocess, 4096, 50, "helloworld3", 1, 3));
	resume(create(myprocess, 4096, 50, "helloworld4", 1, 4));

	while(TRUE) {
        	// Do nothing
	}

	/*
	while (TRUE) {
		//receive();
		//sleepms(200);
		//kprintf("\n\nMain process recreating shell\n\n");
		//resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
		resume(create(myprocess, 4096, 50, "helloworld", 1, 1));
	}
	*/

	return OK;
	
    
}
