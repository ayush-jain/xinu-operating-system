/*  prod_consf.c  - definitions for producer and consumer lab		 */

#include <xinu.h>
#include "prod_cons.h"

char shared_queue[BUFFERSIZE];
pid32 producer_pids[NPRODUCERS];
pid32 consumer_pids[NCONSUMERS]; 

int head;
int tail;
int consumption_stat[NCONSUMERS][NPRODUCERS];
int produced_count[NPRODUCERS];
int final_buffer_count[NPRODUCERS];
int final_size;
int producer_inx[256];

sid32 use_queue;
sid32 empty_count;
sid32 full_count;


int calculate_queue_size(void) {
	int current_size = 0;
	if (head==tail && head !=-1) {
		current_size = 1;		
		// queue full
	} else if (head==tail && head==-1) {
		current_size = 0;		
		// queue empty
	} else if (tail<head) {
		current_size = (tail) + 1 + (BUFFERSIZE - head);   
	} else if (tail>head) {
		current_size = (tail-head) + 1;
	}
	return current_size;
}

void insert_queue(int inx){
	int j;
	
	wait(use_queue);

	//wait(empty_count);
	
	//wait(use_queue);

	int current_size,counter=0;
	int remaining_turns;
	
	current_size = calculate_queue_size();
	
	/*
	if (current_size + producer_counts[inx] > BUFFERSIZE) {
		kprintf("Queue is full - Cant add %c - Producer Process %d going to block\n", producer_tags[inx], inx); 		
		wait(empty_count);
	}
	*/

	if (current_size == 0 && producer_counts[inx] > 0) {
		head=0;
	}
	remaining_turns = producer_counts[inx];

	for (j=tail+1;j<(tail+1)+remaining_turns;j++){
		
		if (current_size >= BUFFERSIZE) {
			//kprintf("Queue is full - Cant add %c head %d tail %d- Producer Process %d going to block\n", producer_tags[inx], head, tail, inx);                 
			tail = (j%BUFFERSIZE)-1;
			if (tail == -1){
				tail = BUFFERSIZE-1;
			}
			if (current_size == 1){
				head = tail;
			}
			if (tail!= -1 && head == -1){
				head=0;
			}

			kprintf("Queue is full - Cant add %c ----- Producer Process %d going to block\n", producer_tags[inx], inx);
			//signal(use_queue);
			//signal(full_count);
			signal(use_queue);
			wait(empty_count);
			wait(use_queue);
			if (current_size == 0) {
				head=0;
			} 
			j=tail+1;
			remaining_turns = remaining_turns - counter;
			counter = 0;
			current_size = calculate_queue_size();
			/*
			if (current_size >= BUFFERSIZE) {
				signal(full_count);
				signal(use_queue);
				wait(empty_count);
				wait(use_queue);
			}
			*/
		} else {
			wait(empty_count);
		}
		
		if (current_size < BUFFERSIZE){
			//wait(empty_count);
			shared_queue[j%BUFFERSIZE] = producer_tags[inx];
			kprintf("Size before addition--%d----Producer Process %d adding %c\n", current_size, inx, producer_tags[inx]);
			produced_count[inx]++;
			current_size++;
			counter++;
			final_size = current_size;
		} else {
		 	j--;
		}

		signal(full_count);
	}
	tail = (j%BUFFERSIZE)-1;
	if (tail == -1){
		tail = BUFFERSIZE-1;
	}
	if (current_size == 1){
		head=tail;
	}
	if (tail!=-1 && head == -1){
		head = 0;
	}
	kprintf("Producer Process %d finished a set---Current Size  %d---Head %d ----Tail %d\n\n", inx, current_size, head, tail);

	//signal(use_queue);

	//signal(full_count);
	//signal(empty_count);

	signal(use_queue);
}	

void delete_queue(int inx){
	int j;
	int producer_index;

	wait(use_queue);

	//wait(full_count);

	//wait(use_queue);

	int current_size, counter;
	int remaining_turns;

	current_size = calculate_queue_size();
	counter = 0;
	remaining_turns = consumer_counts[inx];

	/*
	if (current_size - consumer_counts[inx] < 0) {
		kprintf("Queue is empty - Cant consume - Consumer Process %c going to block\n", consumer_tags[inx]);		
		wait(full_count);
	}
	*/

	for (j=head;j<head+remaining_turns;j++){

		if (current_size  <= 0) {
			//kprintf("Queue is empty - Cant consume - head %d tail %d Consumer Process %c going to block\n", head, tail, consumer_tags[inx]);            
			head = -1;
			tail = -1;
			kprintf("Queue is empty - Can't consume --- Consumer Process %c going to block\n", consumer_tags[inx]);
			//signal(empty_count);
			signal(use_queue);
			wait(full_count);
			wait(use_queue);
			j = head;
			remaining_turns = remaining_turns-counter;
			counter = 0;
			current_size = calculate_queue_size();
			/*
			if(current_size <= 0){
			        signal(empty_count);
				signal(use_queue);
				wait(full_count);
				wait(use_queue);
			}
			*/
		} else {
			wait(full_count);
		}
		if(current_size>0){
			//wait(full_count);
			producer_index = producer_inx[shared_queue[j%BUFFERSIZE]-'0'];		
			kprintf("Size before consumption %d---Consumer Process %c consuming %c\n", current_size, consumer_tags[inx], producer_tags[producer_index]);		
			consumption_stat[inx][producer_index]++;
			current_size--;
			counter++;
			final_size = current_size;
		} else {
			j--;
		}

		signal(empty_count);
	}
	if (current_size == 0){
		head = -1;
		tail = -1;
	} else {
		head = j%BUFFERSIZE;
	}
	kprintf("Consumer Process %c finished a set --Current Size %d---Head %d ----Tail %d\n\n", consumer_tags[inx],  current_size, head, tail);

	//signal(use_queue);

	//signal(empty_count);
	//signal(full_count);

	signal(use_queue);
}

void produce(int inx){
	while(TRUE) {
		insert_queue(inx);
		sleep(producer_sleep_times[inx]/1000);
	}
}

void consume(int inx) {
	while(TRUE) {	
		delete_queue(inx);
		sleep(consumer_sleep_times[inx]/1000);
	}
}

// Mapping where index of producer_inx represents a producer tag and value inside the array represents the index of the corresponding producer 
void reverse_map(void) {
	int k;
	for (k=0;k<NPRODUCERS;k++) {
		producer_inx[producer_tags[k]-'0'] = k;
	}
}


/*------------------------------------------------------------------------
 *  start_prod_con  -  Initializes and starts the producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void start_prod_con(void)
{	
	int i;
	int j;

	head = -1;
	tail = -1;

	final_size = 0;

	kprintf("\n\nProducers - %d, Consumers - %d, Buffersize - %d\n\n", NPRODUCERS, NCONSUMERS, BUFFERSIZE);
	kprintf("---------FINAL REPORT AT THE END OF SIMULATION----------\n");

	for(i=0;i<BUFFERSIZE;i++) {
		shared_queue[i] = '-';
	}	

	reverse_map();

	for (i=0;i<NPRODUCERS;i++) {
		produced_count[i] = 0;
		final_buffer_count[i] = 0;
		for (j=0;j<NCONSUMERS;j++) {
			consumption_stat[j][i]=0;
		}
	}

	use_queue = semcreate(1);
	empty_count = semcreate(BUFFERSIZE);
	full_count = semcreate(0);

	for (i=0;i<NPRODUCERS;i++) {
		producer_pids[i] = create(produce, 4096, 20, &producer_tags[i], 1, i);
		resume(producer_pids[i]);
	}
	for (i=0;i<NCONSUMERS;i++) {
		consumer_pids[i] = create(consume, 4096, 20, &consumer_tags[i], 1, i);
		resume(consumer_pids[i]);
	}	
}

/*------------------------------------------------------------------------
 *  stop_prod_con  -  Stops the currently executing producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void stop_prod_con(void)
{
	int i;

 	for (i=0;i<NPRODUCERS;i++) {
		kill(producer_pids[i]);
		kprintf("Killing producer process %c\n", producer_tags[i]);
	}
	
	for (i=0;i<NCONSUMERS;i++) {
		kill(consumer_pids[i]);
		kprintf("Killing consumer process %c\n", consumer_tags[i]);	
	}
}

/*------------------------------------------------------------------------
 *  print_report  -  Prints the final report for the producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void print_report(void)
{
	int i;
	int j;
	int producer_index;
	int done = 0;
	int current_size;

	kprintf("---------------REPORT---------------\n");
	
	for (i=0;i<NPRODUCERS;i++) {
		kprintf("Producer %c: created %d items\n", producer_tags[i], produced_count[i]);
	}
	
	kprintf("\n");
	
	for (i=0;i<NCONSUMERS;i++) {
		kprintf("Consumer %c: deleted ", consumer_tags[i]);
		for (j=0;j<NPRODUCERS;j++) {
			kprintf("%d items from producer %c ", consumption_stat[i][j], producer_tags[j]);
		}
		kprintf("\n");
	}

	current_size = calculate_queue_size();
	if (final_size > current_size){
		tail=(tail + (final_size-current_size))%BUFFERSIZE;
	} else if (final_size < current_size) {
		head = (head + (current_size-final_size))%BUFFERSIZE;
	}

	if (final_size > 0 && head== -1){
		head = 0;
	}

	if(final_size == 0){
		head = -1;
		tail = -1;
	}

	if(final_size != 0){
		for (i=head;;i++) {
			if(done == 1){
				break;
			}
			producer_index = producer_inx[shared_queue[i%BUFFERSIZE]-'0'];
			final_buffer_count[producer_index]++;
			if(i%BUFFERSIZE == tail){
				done = 1;
			}	
		}
	}
	kprintf("%d---%d\n", final_size, current_size);
	kprintf("The shared buffer contains: ");
    	for (i=0;i<NPRODUCERS;i++) {
		kprintf("%d items from producer %c ", final_buffer_count[i], producer_tags[i]);	
	}
	kprintf("\n");
}
