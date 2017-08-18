/* myreceive.c - receive and myreceiven */

#include <xinu.h>

extern int calculate_queue_size(int32 head, int32 tail);

uint32 delete_queue(struct procent *prptr, uint32 count, int current_size, umsg32* msgs){
	int j;
	uint32 counter = 0; 	

	for (j=prptr->head;j<(prptr->head)+count;j++){
		if(current_size>0){
			msgs[counter] = prptr->buffer[j%BUFFERSIZE];
			//kprintf("printing indiv. %c\n", msgs[counter]);
			current_size--;
			counter++;
		} else {
			kprintf("receiver_time can't delete until buffer is non-empty\n");		
			break;
		}
	}
	if (current_size == 0){
		prptr->head = -1;
		prptr->tail = -1;
	} else {
		prptr->head = j%BUFFERSIZE;
	}
	return counter;
}

/*------------------------------------------------------------------------
 *  myreceive  -  Wait for a message from the queue and return the 
 *			message to the caller
 *------------------------------------------------------------------------
 */
umsg32	myreceive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	umsg32	msg = SYSERR;		/* Message to return		*/
	int current_size = 0;
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	
	/* Additional code for myreceive goes here */	
	prptr = &proctab[currpid];	
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	prptr->wait_count = 1;
	int result = current_size-(prptr->wait_count);
	if (result<0){
		kprintf("Blocking receiver until message is received\n");
		prptr->prstate = PR_MYRECV;
		resched();		/* Block until message arrives	*/
	}
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Receiver ready to receive --head %d ---tail %d---current size %d ----wait count %d----result %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count, result);
	delete_queue(prptr, 1, current_size, &msg);
	prptr->wait_count = 0;
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Receiver received message --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	
	kprintf("Received message %c\n", msg);
	restore(mask);			/* Restore interrupts */
	return msg;
}

/*------------------------------------------------------------------------
 *  myreceiven  -  Wait for 'n' messages from the queue and return the 
 *			messages to the caller
 *------------------------------------------------------------------------
 */
syscall	myreceiven(
	  umsg32*	msgs,		/* Contents of messages		*/
	  uint32	msg_count	/* Number of messages to receive*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int current_size = 0;
	int i;	

	mask = disable();
	
	/* Additional code for myreceiven goes here */
	prptr = &proctab[currpid];	
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	prptr->wait_count = msg_count;
	int result = current_size - (prptr->wait_count);	
	if (result < 0){
		prptr->prstate = PR_MYRECV;
		kprintf("Blocking receiver_multiple until messages are received\n");
		resched();		/* Block until message arrives	*/
	}
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Receiver_multiple ready to receive --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	delete_queue(prptr, msg_count, current_size, msgs);
	prptr->wait_count = 0;
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Receiver_multiple received message --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	
	for (i=0;i<msg_count;i++) {
		kprintf("Received_multiple message %c\n", msgs[i]);
	}
	kprintf("Receive_multiple count %d\n", msg_count);	

	restore(mask);			/* Restore interrupts */
	return OK;
}

uint32	myrecvtime(
		int32 maxwait, 
		umsg32* msgs, 
		uint32 msg_count
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent	*prptr;		/* Tbl entry of current process	*/
	uint32 count = 0;
	int current_size = 0;
	int i;

	if (maxwait < 0) {
		return SYSERR;
	}
	mask = disable();

	/* Schedule wakeup and place process in timed-receive state */

	prptr = &proctab[currpid];
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	prptr->wait_count = msg_count;
	int result = current_size - (prptr->wait_count);	
	if (result < 0){
		//kprintf("Start--Blocking receiver_time until messages are received or timeout occurs\n");
		if (insertd(currpid,sleepq,maxwait) == SYSERR) {
			restore(mask);
			return SYSERR;
		}
		prptr->prstate = PR_MYRECTIM;
		kprintf("Blocking receiver_time until messages are received or timeout occurs\n");
		resched();		/* Block until message arrives or timeout occurs	*/
	
	}
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Receiver_time ready to receive --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	count = delete_queue(prptr, msg_count, current_size, msgs);
	prptr->wait_count = 0;
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Receiver_time received message --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	
	for (i=0;i<count;i++) {
		kprintf("Received message %c\n", msgs[i]);
	}
	kprintf("Receive_time count %d\n", count);

	restore(mask);			/* Restore interrupts */
	return count;

}
