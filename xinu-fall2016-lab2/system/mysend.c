/* mysend.c - mysend and mysendn */

#include <xinu.h>

int calculate_queue_size(int32 head, int32 tail) {
	int size = 0;
	if (head==tail && head !=-1) {
		size = 1;		
		// queue full
	} else if (head==tail && head==-1) {
		size = 0;		
		// queue empty
	} else if (tail<head) {
		size = (tail) + 1 + (BUFFERSIZE - head);   
	} else if (tail>head) {
		size = (tail-head) + 1;
	}
	return size;
}

uint32 insert_queue(struct procent *prptr, umsg32* msg, uint32 count, int current_size){

	int j;
	uint32 counter = 0;
	umsg32* buffer = prptr->buffer;
	
	if (current_size == 0 && count > 0) {
		prptr->head=0;
	}

	for (j=(prptr->tail)+1;j<(prptr->tail)+1+count;j++){
		if (current_size < BUFFERSIZE){
			buffer[j%BUFFERSIZE] = msg[counter]; 
			current_size++;
			counter++;
		} else {
			kprintf("Buffer got full while sending messages\n");
			break;
		}
	}
		
	prptr->tail = (j%BUFFERSIZE)-1;
	if (prptr->tail == -1){
		prptr->tail = BUFFERSIZE-1;
	}
	if (current_size == 1){
		prptr->head=prptr->tail;
	}
	if (prptr->tail!=-1 && prptr->head == -1){
		prptr->head = 0;
	}

	return counter;
}
	

/*------------------------------------------------------------------------
 *  mysend  -  Pass a message to a process's message queue
 *			and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	mysend(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int current_size = 0;

	mask = disable();

	if (isbadpid(pid)) {
		kprintf("Error: Bad receiver pid\n");		
		restore(mask);
		return SYSERR;
	}

	/* Additional code for mysend goes here */
	prptr = &proctab[pid];	
	current_size = calculate_queue_size(prptr->head, prptr->tail);

	if (current_size>=BUFFERSIZE) {
		kprintf("Buffer already full\n");
		restore(mask);
		return SYSERR;
	}
	kprintf("Sender ready to send message --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	insert_queue(prptr, &msg, 1, current_size);
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Sender sent message --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);

	/* If recipient waiting, make it ready */

	if (prptr->prstate == PR_MYRECV && current_size >= (prptr->wait_count)) {
		kprintf("Making receipient ready\n");
		ready(pid);
	} else if (prptr->prstate == PR_MYRECTIM && current_size >= (prptr->wait_count)) {
		kprintf("Making receipient ready from rectim state\n");		
		unsleep(pid);
		ready(pid);
	}
	kprintf("Sent message %c\n", msg);
	restore(mask);		/* Restore interrupts */
	return OK;
}

/*------------------------------------------------------------------------
 *  mysend  -  Pass 'n' messages to a process's message queue 
 #			and start recipient if waiting
 *------------------------------------------------------------------------
 */
uint32	mysendn(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32*	msgs,		/* Contents of messages		*/
	  uint32	msg_count	/* Number of messages to send	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32	msgs_sent = 0;		/* The number of messages sent	*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int current_size = 0;
	int i;

	mask = disable();
	if (isbadpid(pid)) {
		kprintf("Error: Bad receiver pid\n");
		restore(mask);
		return SYSERR;
	}
	
	/* Additional code for mysendn goes here */
	prptr = &proctab[pid];	
	current_size = calculate_queue_size(prptr->head, prptr->tail);

	if (current_size>=BUFFERSIZE) {
		kprintf("Buffer already full for send_multiple\n");
		restore(mask);
		return 0;
	}
	
	kprintf("Sender_multiple ready to send message --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);
	msgs_sent = insert_queue(prptr, msgs, msg_count, current_size);	
	current_size = calculate_queue_size(prptr->head, prptr->tail);
	kprintf("Sender_multiple sent messages --head %d ---tail %d---current size %d ----wait count %d\n", prptr->head, prptr->tail, current_size, prptr->wait_count);

	/* If recipient waiting, make it ready */

	if (prptr->prstate == PR_MYRECV && current_size >= (prptr->wait_count)) {
		kprintf("Making receipient ready from send_multiple\n");		
		ready(pid);
	} else if (prptr->prstate == PR_MYRECTIM && current_size >= (prptr->wait_count)) {
		unsleep(pid);
		ready(pid);
	}

	for (i=0;i<msgs_sent;i++) {
		kprintf("%c Sent message\n", msgs[i]);
	}
	kprintf("Sent_multiple count %d\n", msgs_sent);

	restore(mask);		/* Restore interrupts */
	return msgs_sent;
}
