/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/


void receive_call(int inx){
	char msg = receive();
	kprintf("%c msg\n", msg);
}

void send_call(int inx, pid32 pid, umsg32 msg){	
	send(pid, msg);
}

void normal_process(void){
	while(1){
			
	}
}

//receiver waiting to receive message with delayed suspend flag set . Sender sends message leading to suspension of receiver and receiver is finally resumed 
void start_testcase_1(void){

	char sender[10] = "Sender";
	char receiver[10] = "Receiver";
	
	char sent_msg = 'a';
	
	pid32 pid = create(receive_call, 4096, 20, receiver, 1, 0);
	resume(pid);
	
	sleepms(200);
	
	mysuspend(pid);

	pid32 pid1 = create(send_call, 4096, 20, sender, 3, 0, pid, sent_msg);
	resume(pid1);
	
	sleep(5);

	resume(pid);
}

// normal process suspended followed by another mysuspend call resulting in error
void start_testcase_2(void){

	char name[10] = "normal";

	pid32 pid = create(normal_process, 4096, 20, name, 1, 0);
	resume(pid);
	
	sleepms(200);
	
	mysuspend(pid);

	int status = mysuspend(pid);
	kprintf("Status %d\n", status);

	resume(pid);
}

//suspending current process
void start_testcase_3(void){

	mysuspend(currpid);
}

//multiple mysuspend calls to a process with delayed suspend flag already set
void start_testcase_4(void){

	char receiver[10] = "Receiver";
	
	pid32 pid = create(receive_call, 4096, 20, receiver, 1, 0);
	resume(pid);
	
	sleepms(200);
	
	mysuspend(pid);
	
	sleep(1);

	int status = mysuspend(pid);
	kprintf("Status %d\n", status); 
}

//existing system calls
void start_testcase_5(void){

	char receiver[10] = "Receiver";
	
	pid32 pid = create(receive_call, 4096, 20, receiver, 1, 0);
	resume(pid);
	
	sleepms(200);

	int status = suspend(pid);
	kprintf("Status %d\n", status);
	
	sleep(5);

	resume(pid);
}

/***EXTRA CREDIT************/

//receiver is in PR_RECV state with delayed sleep state. As soon as sender sends a message, receiver goes to suspend state. Final resume called to resume it from suspended state
void start_testcase_6(void){

	char sender[10] = "Sender";
	char receiver[10] = "Receiver";
	
	char sent_msg = 'a';
	
	pid32 pid = create(receive_call, 4096, 20, receiver, 1, 0);
	resume(pid);

	mysleepms(pid, 10000);

	pid32 pid1 = create(send_call, 4096, 20, sender, 3, 0, pid, sent_msg);
	resume(pid1);
	
	sleep(1);

	resume(pid);
}

//current process calling mysleepms to execute normal sleep behaviour
void start_testcase_7(void){
	//kprintf("Starting mysleepms test...\n");
	mysleepms(currpid, 10000);
	//kprintf("Should return now\n");
}

//current process calling mysleepms on receiver multiple times with delay > 0 resulting in SYSERR output, second time it is called
void start_testcase_8(void){
	char receiver[10] = "Receiver";
		
	pid32 pid = create(receive_call, 4096, 20, receiver, 1, 0);
	resume(pid);
	sleepms(1000);		

	mysleepms(pid, 10000);
	
	int status = mysleepms(pid, 10000);
	kprintf("Status %d\n", status);
}

//receiver is in PR_RECV state with delayed sleep flag set. mysleepms is called again with delay=0 resulting in SYSERR output
void start_testcase_9(void){

	char receiver[10] = "Receiver";
	
	pid32 pid = create(receive_call, 4096, 20, receiver, 1, 0);
	resume(pid);

	sleepms(1000);
	mysleepms(pid, 10000);

	int status = mysleepms(pid, 0);	
	kprintf("Status  %d\n", status);

}

// process first sleeping using mysleepms. Then mysleep called on process with delay = 0 resulting in SYSERR output
void start_testcase_10(void){
	char name[10] = "Normal";
		
	pid32 pid = create(normal_process, 4096, 20, name, 1, 0);
	resume(pid);

	sleepms(1000);

	mysleepms(pid, 20000);
	int status = mysleepms(pid, 0);
	kprintf("Status %d\n", status);
}

	

process	main(void)
{
	/* Place your test cases here */
	/*  NOTE: Remove the code to start a shell when you create your */
	/*        test cases.                                           */
	/*  NOTE: Do not forget to modify xsh_ps.c                      */

	/* Run the Xinu shell */

	/*Uncomment each specific test case*/

	start_testcase_1();
	
	//start_testcase_2();

	//start_testcase_3();

	//start_testcase_4();

	//start_testcase_5();

	//start_testcase_6();

	//start_testcase_7();

	//start_testcase_8();

	//start_testcase_9();

	//start_testcase_10();



	//recvclr();
	//resume(create(shell, 8192, 50, "shell", 1, CONSOLE));
	
	/* Wait for shell to exit and recreate it */
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
