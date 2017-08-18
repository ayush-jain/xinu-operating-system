/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

#define BUFSIZE  20

int restart_flag = 0;
char* record_buf1[BUFSIZE];
bpid32 record_bufpool1[NBPOOLS];
int buf_size1 = 0;
int bufpool_size1 = 0;

char* record_buf2[BUFSIZE];
bpid32 record_bufpool2[NBPOOLS];
int buf_size2 = 0;
int bufpool_size2 = 0;

bpid32 pool1;

//All debugging statements only in main.c for each testcase not anywhere else

void func_call(int c, int d){
	kprintf("Creating new process func_call %d....%d\n", c, d);
	sleep(50);
	kprintf("Ending func_call...\n");
}

// Each sample process clean up function keeps track of buffers retrived and buffer pool allocated in global variables and does the cleanup accordingly based on this info.
void process_cleanup_func1(){
	kprintf("Cleaning buf-1 process.....---\n");
	int i=0;
	for(i=0;i<BUFSIZE;i++){
		if(record_buf1[i] != NULL){
			freebuf(record_buf1[i]);
		}	
	}
	//As a part of process cleanup function, if buffers freed by this process makes bufferpool of this process empty then this pool is freed else not. Status variable is used to check status of free buf pool.
	for(i=0;i<NBPOOLS;i++){
		if(record_bufpool1[i] != -1){
			int status = freebufpool(record_bufpool1[i]);
			kprintf("Status is %d\n", status);
		}
	}
}

void process_cleanup_func2(){
	kprintf("Cleaning buf-2 or buf-3.....---\n");
	int i=0;
	for(i=0;i<BUFSIZE;i++){
		if(record_buf2[i] != NULL){
			freebuf(record_buf2[i]);
		}	
	}
	for(i=0;i<NBPOOLS;i++){
		if(record_bufpool2[i] != -1){
			int status = freebufpool(record_bufpool2[i]);
			kprintf("Status is %d\n", status);
		}
	}
}

void buf_call1(){

	kprintf("Creating buf-1 process with allocation of bufferpool and gets 2 buffers for that pool\n");
	regcleanup(process_cleanup_func1);
	sleep(5);

	pool1 = mkbufpool(512, 4);
	record_bufpool1[bufpool_size1++] = pool1;
	char* buf1 = getbuf(pool1);
	record_buf1[buf_size1++] = buf1;
	char* buf2 = getbuf(pool1);
	record_buf1[buf_size1++] = buf2;

	sleep(10);
}

void buf_call2(){

	kprintf("Creating buf-2 process with allocation of buffer in buffer pool1\n");
	regcleanup(process_cleanup_func2);
	
	sleep(10);

	char* buf2 = getbuf(pool1);
	record_buf2[buf_size2++] = buf2;

}


void buf_call3(){

	kprintf("Creating buf-3 process with any allocation of buffer in buffer pool1\n");
	regcleanup(process_cleanup_func2);
	sleep(10);

}

void start_test_case_3(){

	int i;
	if(restart_flag == 0){
		resume(create((void *)buf_call1, 4096, 20,
						"Buf 1 process", 0));

		resume(create((void *)buf_call2, 4096, 20,
						"Buf 2 process", 0));


		restart_flag = 1;

		restart(3);
	}

	sleep(2);
	
	//The below information is used to check whether new processes are created as per the killing of the original user processes
	for(i=0;i<NPROC;i++){
		if(infotab[i].nargs != -1){
			kprintf("Final Info about all the existing processes.....%d\n", i);
			kprintf("%d..\n", infotab[i].nargs);
			if(sys_proc[i] == TRUE)
				kprintf("System Process..bool\n");
			else 
				kprintf("New User Processes after restart...bool\n");		
		}
	}
	
}

void start_test_case_2(){
	int i ;
	if(restart_flag == 0){
		
		kprintf("hi..\n");

		resume(create((void *)func_call, 4096, 20,
						"New process", 2, 1, 2));
		sleep(2);
		kprintf("slept\n");

		for(i=0;i<NPROC;i++){
			if(infotab[i].nargs != -1){
				kprintf("info.....%d\n", i);
				kprintf("%d..\n", infotab[i].nargs);
				if(sys_proc[i] == TRUE)
					kprintf("11111..bool\n");
				else 
					kprintf("0000...bool\n");		
			}
		}

		restart_flag = 1;
		

		struct memblk *cur = head;
		int count = 0;
		kprintf("Freelist Snapshot size is...%d\n\n", snapshot_size);
	
		while(count < snapshot_size){
			kprintf("%p\n", (void *)cur->mnext);
			kprintf("%u\n\n\n", cur->mlength);
			cur=cur+1;
			count++;
		}
		
		restart(7);	
	}
	
	sleep(20);
	
	for(i=0;i<NPROC;i++){
		if(infotab[i].nargs != -1){
			kprintf("%d..\n", infotab[i].nargs);
			if(sys_proc[i] == TRUE)
				kprintf("System Process..bool\n");
			else 
				kprintf("New User Processes after restart...bool\n");		
		}
	}
	
}

// Testing [freebufpool+availbufs+regcleanup]  independently without restart when process terminates naturally. This also tests existing system calls like mkbufpool,getbuf etc. with new logic for new buf poolid
void start_test_case_1(){

	int i;
	
	resume(create((void *)buf_call1, 4096, 20,
						"Buf-1 process", 0));

	resume(create((void *)buf_call2, 4096, 20,
						"Buf-2 process", 0));

	//buf_call2 can be replaced by buf_call3 for successful deallocation of bufferpool when buffers are not shared across multiple porcesses.
	
	////The below information is used to check whether new processes are created as per the killing of the original user processes
	for(i=0;i<NPROC;i++){
		if(infotab[i].nargs != -1){
			kprintf("Final Info about all the existing processes.....%d\n", i);
			kprintf("%d..\n", infotab[i].nargs);
			if(sys_proc[i] == TRUE)
				kprintf("System Process..bool\n");
			else 
				kprintf("New User Processes after restart...bool\n");		
		}
	}

	sleep(2);
	
}

process	main(void)
{
	int i=0;
	for(i=0;i<BUFSIZE;i++){
		record_buf1[i] = NULL;
		record_buf2[i] = NULL;
	}

	for(i=0;i<NBPOOLS;i++){
		record_bufpool1[i] = -1;
		record_bufpool2[i] = -1;
	}

	//Uncomment each specific test case for testing the functionality given in the description of each test case.

	//start_test_case_1();

	//start_test_case_2();

	//start_test_case_3();

	//Extra credit implementation like freelist_snapshot and restore_snapshot(commented in restart call) already tested during initialization and debugging info in above test cases. 
	

	return OK;

}