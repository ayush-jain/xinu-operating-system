/*  main.c  - main */

#include <xinu.h>

#define VECSIZE		200

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

int32 ta_tc_restart_flag = 0;

bpid32 data_pool;

uint32* mainbuf1;
uint32* mainbuf2;

uint32* myproc1buf;
uint32* myproc2buf;

sid32 myproc1sem;
sid32 myproc2sem;

void maincleanup(void)
{
	int32 avail_bufs = 0;
	int32 total_bufs = 0;
	
	kprintf("CLEANUP MAIN\n");
	
	semdelete(myproc1sem);
	semdelete(myproc2sem);
	
	if(freebuf((char*)mainbuf1) == SYSERR) {
		kprintf("MAIN CLEANUP: freebuf(mainbuf1) failed\n");
		return;
	}
	if(freebuf((char*)mainbuf2) == SYSERR) {
		kprintf("MAIN CLEANUP: freebuf(mainbuf2) failed\n");
		return;
	}

	avail_bufs = bufsavail(data_pool, &total_bufs);
	if(avail_bufs == SYSERR) {
		kprintf("MAIN CLEANUP: bufsavail failed\n");
		return;
	}
	kprintf("MAIN CLEANUP: AB %d TB %d\n", avail_bufs, total_bufs);

	if(avail_bufs == total_bufs) {
		kprintf("MAIN CLEANUP: freeing buffer pool\n");
		if(freebufpool(data_pool) == SYSERR) {
			kprintf("MAIN CLEANUP: freebufpool failed\n");
			return;
		}
		kprintf("MAIN CLEANUP: buffer pool freed\n");
	}
}

void myproccleanup1(void)
{
	int32 avail_bufs = 0;
	int32 total_bufs = 0;
	
	kprintf("CLEANINGUP 1\n");
	
	if(freebuf((char*)myproc1buf) == SYSERR) {
		kprintf("CLEANINGUP 1: freebuf(myproc1buf) failed\n");
		return;
	}

	avail_bufs = bufsavail(data_pool, &total_bufs);
	if(avail_bufs == SYSERR) {
		kprintf("CLEANINGUP 1: bufsavail failed\n");
		return;
	}
	kprintf("CLEANINGUP 1: AB %d TB %d\n", avail_bufs, total_bufs);

	if(avail_bufs == total_bufs) {
		kprintf("CLEANINGUP 1: freeing buffer pool\n");
		if(freebufpool(data_pool) == SYSERR) {
			kprintf("CLEANINGUP 1: freebufpool failed\n");
			return;
		}
		kprintf("CLEANINGUP 1: buffer pool freed\n");
	}
	signal(myproc1sem);
}

void myproccleanup2(void)
{
	int32 avail_bufs = 0;
	int32 total_bufs = 0;
	
	kprintf("CLEANINGUP 2\n");
	
	if(freebuf((char*)myproc2buf) == SYSERR) {
		kprintf("CLEANINGUP 2: freebuf(myproc1buf) failed\n");
		return;
	}

	avail_bufs = bufsavail(data_pool, &total_bufs);
	if(avail_bufs == SYSERR) {
		kprintf("CLEANINGUP 2: bufsavail failed\n");
		return;
	}
	kprintf("CLEANINGUP 2: AB %d TB %d\n", avail_bufs, total_bufs);

	if(avail_bufs == total_bufs) {
		kprintf("CLEANINGUP 2: freeing buffer pool\n");
		if(freebufpool(data_pool) == SYSERR) {
			kprintf("CLEANINGUP 2: freebufpool failed\n");
			return;
		}
		kprintf("CLEANINGUP 2: buffer pool freed\n");
	}
	signal(myproc2sem);
}

void myproc1(uint32* a, uint32* b)
{
	int32 i;
	
	kprintf("START PROC 1\n");
	kprintf("MYPROC1: REGCLEANUP\n");
	regcleanup(myproccleanup1);
	kprintf("MYPROC1: RUNNING\n");

	myproc1buf = (uint32*)getbuf(data_pool);
	for(i = 0; i < VECSIZE; i++) {
		myproc1buf[i] = a[i] + b[i];
		sleepms(10);
	}

	kprintf("MYPROC1 RESULT:\n");
	for(i = 0; i < VECSIZE; i++) {
		kprintf("%d ", myproc1buf[i]);
	}
	kprintf("\n");
}

void myproc2(uint32* a, uint32* b)
{
	int32 i;
	
	kprintf("START PROC 2\n");
	kprintf("MYPROC2: REGCLEANUP\n");
	regcleanup(myproccleanup2);
	kprintf("MYPROC2: RUNNING\n");

	myproc2buf = (uint32*)getbuf(data_pool);
	for(i = 0; i < VECSIZE; i++) {
		myproc2buf[i] = a[i] * b[i];
		sleepms(10);
	}

	kprintf("MYPROC2 RESULT:\n");
	for(i = 0; i < VECSIZE; i++) {
		kprintf("%d ", myproc2buf[i]);
	}
	kprintf("\n");
}

process	main(void)
{
	int32 i;

	pid32 proc1pid;
	pid32 proc2pid;

	kprintf("MAIN STARTED\n");

	kprintf("MAIN REGCLEANUP\n");
	regcleanup(maincleanup);
	
	data_pool = mkbufpool(sizeof(uint32) * VECSIZE, 4);

	myproc1sem = semcreate(0);
	myproc2sem = semcreate(0);

	mainbuf1 = (uint32*)getbuf(data_pool);
	mainbuf2 = (uint32*)getbuf(data_pool);

	for(i = 0; i < VECSIZE; i++) {
		mainbuf1[i] = i;
		mainbuf2[i] = VECSIZE - i;
	}
	
	kprintf("MAINBUF1\n");
	for(i = 0; i < VECSIZE; i++) {
		kprintf("%d ", mainbuf1[i]);
	}
	kprintf("\nMAINBUF2\n");
	for(i = 0; i < VECSIZE; i++) {
		kprintf("%d ", mainbuf2[i]);
	}
	kprintf("\n");

	proc1pid = create(myproc1, 4096, 6, "MYPROC1", 2, mainbuf1, mainbuf2);
	proc2pid = create(myproc2, 4096, 5, "MYPROC2", 2, mainbuf1, mainbuf2);

	resume(proc1pid);
	resume(proc2pid);

	wait(myproc1sem);
	wait(myproc2sem);

	kprintf("MAIN EXIT\n");

	return OK;
}
