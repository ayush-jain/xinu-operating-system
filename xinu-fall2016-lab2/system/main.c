/*  main.c  - main */

#include <xinu.h>
#define MAXSENDERS	50
#define MAXRECEIVERS 50
#define MAXSIZE	50

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

umsg32 sent_msg;
umsg32* sent_msgs;
umsg32* received_msgs;
umsg32 received_msg;
umsg32 msgs[MAXSIZE];
umsg32 msgs2[MAXSIZE];
umsg32 rec_msgs[MAXSIZE];

pid32 sender_pids[MAXSENDERS];
pid32 receiver_pids[MAXRECEIVERS];



void receive_call(int inx){
	kprintf("Calling receiver %d..\n", inx);
	received_msg = myreceive();
}

void receive_existing(int inx){
	kprintf("Calling existing receiver %d..\n", inx);
	received_msg = receive();
	kprintf("Received Message from receive() %c\n", received_msg);
} 

void send_call(int inx, pid32 pid, umsg32 msg){
	kprintf("Calling sender %d..\n", inx);	
	mysend(pid, msg);
}

void send_existing(int inx, pid32 pid, umsg32 msg){
	kprintf("Calling existing sender %d..\n", inx);	
	send(pid, msg);
}

void receive_existing_time(int inx, int32 maxwait){
	kprintf("Calling existing recvtime %d\n", inx);
	received_msg = recvtime(maxwait);
	kprintf("Received Message from recvtime() %c\n", received_msg);
}

void send_multiple(int inx, pid32 pid, umsg32 msg_count){
	kprintf("Calling send_multiple %d..\n", inx);		
	mysendn(pid, sent_msgs, msg_count);	
}

void receive_multiple(int inx, umsg32 msg_count){
	kprintf("Calling receive_multiple %d..\n", inx);	
	myreceiven(received_msgs, msg_count);
}

void receiven_time(int inx, umsg32 msg_count, uint32 maxwait){
	kprintf("Calling receive_time %d..\n", inx);
	myrecvtime(maxwait, received_msgs, msg_count);		
}


void receive_send(int inx, umsg32 msg_count){
	kprintf("Calling receive_multiple %d..\n", inx);	
	myreceiven(received_msgs, msg_count);

	kprintf("Calling same process send_multiple %d..\n", inx);		
	mysendn(currpid, sent_msgs, msg_count);
	//mysendn will not execute here as the process is blocked waiting for messages
}

void send_itself(int inx, umsg32 msg_count){
	kprintf("Calling same process send_multiple %d..\n", inx);		
	mysendn(currpid, sent_msgs, msg_count);
}



//-----------TEST CASES-------


// 1 receiver waits to receive message from 1 sender
void start_test_case_1(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	
	sent_msg = 'a';
	received_msg = '-';
	kprintf("Initial Received message %c\n", received_msg);
	
	receiver_pids[0] = create(receive_call, 4096, 20, &receiver, 1, 0);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_call, 4096, 20, &sender, 3, 0, receiver_pids[0], sent_msg);
	resume(sender_pids[0]);
}

// 1 receiver waits to receive message from 2 senders - 2nd sender gives an error as receiver has already received message from 1st sender
void start_test_case_2(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	
	sent_msg = 'a';
	received_msg = '-';
	kprintf("Initial Received message %c\n", received_msg);
	
	receiver_pids[0] = create(receive_call, 4096, 20, &receiver, 1, 0);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_call, 4096, 20, &sender, 3, 0, receiver_pids[0], sent_msg);
	resume(sender_pids[0]);

	sender_pids[1] = create(send_call, 4096, 20, &sender, 3, 1, receiver_pids[0], sent_msg);
	resume(sender_pids[1]);
}

// 1 sender sends a message and there is no receiver ( -1 pid)
void start_test_case_3(int nsend, int recv){
	char sender = 'S';
	
	sent_msg = 'a';
	received_msg = '-';
	kprintf("Initial Received message %c\n", received_msg);

	sender_pids[0] = create(send_call, 4096, 20, &sender, 3, 0, -1, sent_msg);
	resume(sender_pids[0]);

}

// 1 receiver wait for a message but there is no sender
void start_test_case_4(int nsend, int recv){
	char receiver = 'R';
	
	received_msg = '-';
	kprintf("Initial Received message %c\n", received_msg);

	receiver_pids[0] = create(receive_call, 4096, 20, &receiver, 1, 0);
	resume(receiver_pids[0]);

}

// 2 receivers and 2 senders
void start_test_case_5(int nsend, int recv){
	char sender = 'S';
	char receiver = 'R';
	
	umsg32 sent_msg1 = 'a';
	umsg32 sent_msg2 = 'b';
	received_msg = '-';
	kprintf("Initial Received message for receiver 1 %c\n", received_msg);
	kprintf("Initial Received message for receiver 2 %c\n", received_msg);

	receiver_pids[0] = create(receive_call, 4096, 20, &receiver, 1, 0);
	resume(receiver_pids[0]);

	receiver_pids[1] = create(receive_call, 4096, 20, &receiver, 1, 1);
	resume(receiver_pids[1]);

	sender_pids[0] = create(send_call, 4096, 20, &sender, 3, 0, receiver_pids[0], sent_msg1);
	resume(sender_pids[0]);

	sender_pids[1] = create(send_call, 4096, 20, &sender, 3, 1, receiver_pids[1], sent_msg2);
	resume(sender_pids[1]);	
}

//sending messages to itself with receive first executed
void start_test_case_6(int nsend, int nrecv){

	char receiver_sender = 'B';
	int count = 10;
	
	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		received_msgs[i] = '-';
		kprintf("%c  Initial received message\n", received_msgs[i]);
	}	

	receiver_pids[0] = create(receive_send, 4096, 20, &receiver_sender, 2, 0, count);
	resume(receiver_pids[0]);

}

//sending messages to itself
void start_test_case_7(int nsend, int nrecv){

	char receiver_sender = 'B';
	int count = 15;
	
	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		received_msgs[i] = '-';
		kprintf("%c  Initial received message\n", received_msgs[i]);
	}	

	receiver_pids[0] = create(send_itself, 4096, 20, &receiver_sender, 2, 0, count);
	resume(receiver_pids[0]);

}


// 1 receiver_multiple waits to receive message from 1 sender_multiple ( 10 messages at a time)
void start_test_case_8(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	int count = 10;
	
	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		received_msgs[i] = '-';
		kprintf("%c  Initial received message\n", received_msgs[i]);
	}
	
	receiver_pids[0] = create(receive_multiple, 4096, 20, &receiver, 2, 0, count);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);
}

// 1 receiver(wants to receive 20 messages) and 2 senders (sending 12 messages each)
void start_test_case_9(int nsend, int recv){
	char sender = 'S';
	char receiver = 'R';
	int count = 12;
	
	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		msgs2[i] = 'b';
	}

	for (i=0;i<20;i++){
		received_msgs[i] = '-';
		kprintf("%c  Initial received message\n", received_msgs[i]);
	}
	
	receiver_pids[0] = create(receive_multiple, 4096, 20, &receiver, 2, 0, 20);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);	
	
	sender_pids[1] = create(send_multiple, 4096, 20, &sender, 3, 1, receiver_pids[0], count);
	resume(sender_pids[1]);	
}

// 1 receiver_multiple waits to receive message from 1 sender_multiple ( sender wants to send 23 messages at a time > buffersize)
void start_test_case_10(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	int count = 23;
	
	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		if(i<20){
			received_msgs[i] = '-';
			kprintf("%c  Initial received message\n", received_msgs[i]);
		}
	}
	
	receiver_pids[0] = create(receive_multiple, 4096, 20, &receiver, 2, 0, 20);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);
}

//mysendn and mysend together with mysend sending message to receiver
void start_test_case_11(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	int count = 20;
	sent_msg = 'b';
	
	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		received_msgs[i] = '-';
		kprintf("%c  Initial received message\n", received_msgs[i]);
	}

	receiver_pids[0] = create(receive_multiple, 4096, 20, &receiver, 2, 0, 20);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_call, 4096, 20, &sender, 3, 0, receiver_pids[0], sent_msg);
	resume(sender_pids[0]);

	sender_pids[1] = create(send_multiple, 4096, 20, &sender, 3, 1, receiver_pids[0], count);
	resume(sender_pids[1]);

}

//mysendn sending 20 messages and myreceive receiving 1 message
void start_test_case_12(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	int count = 20;
	
	sent_msgs = msgs;
	
	received_msg = '-';
	kprintf("Initial Received message %c\n", received_msg);	
	
	int i;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
	}

	receiver_pids[0] = create(receive_call, 4096, 20, &receiver, 1, 0);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);
}

//mysend sending 1 message and myreceiven receiving 1 message
void start_test_case_13(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';
	int count = 1;
	
	sent_msg = 'a';
	
	received_msg = '-';
	kprintf("Initial Received message %c\n", received_msg);	
	
	receiver_pids[0] = create(receive_multiple, 4096, 20, &receiver, 2, 0, count);
	resume(receiver_pids[0]);

	sender_pids[0] = create(send_call, 4096, 20, &sender, 3, 0, receiver_pids[0], sent_msg);
	resume(sender_pids[0]);
}

//existing system calls send, receive, recvtime and recvclr()
void start_test_case_14(int nsend, int nrecv){
	
	int status = recvclr();
	kprintf("%d Status of recvclr\n", status);

	char sender = 'S';
	char receiver = 'R';
	
	sent_msg = 'a';
	received_msg = '-';
	umsg32 sent_msg2 = 'b';
	kprintf("Initial Received message %c\n", received_msg);
	
	receiver_pids[0] = create(receive_existing, 4096, 20, &receiver, 1, 0);
	resume(receiver_pids[0]);
	
	receiver_pids[1] = create(receive_existing_time, 4096, 20, &receiver, 2, 1, 2000);
	resume(receiver_pids[1]);

	sender_pids[0] = create(send_existing, 4096, 20, &sender, 3, 0, receiver_pids[0], sent_msg);
	resume(sender_pids[0]);

	sender_pids[1] = create(send_existing, 4096, 20, &sender, 3, 1, receiver_pids[1], sent_msg2);
	resume(sender_pids[1]);
}
		

// ***extra credential***

//test case for myrecvtime and mysendn with very small timeout period of 2ms . As a result, receiver is not able to receive any message and sender can't recognize the pid as receiver has already terminated 
void start_test_case_15(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';

	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	int count = 5;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		kprintf("%c  Initial received message\n", received_msgs[i]);
		received_msgs[i] = '-';
	}
    
	receiver_pids[0] = create(receiven_time, 4096, 20, &receiver, 3, 0, count, 2);
	resume(receiver_pids[0]);
	
	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);
	
}


//test case for myrecvtime and mysendn with very large timeout period of 5000ms so that required amount of messages can be received .  
void start_test_case_16(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';

	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	int count = 5;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
		kprintf("%c  Initial received message\n", received_msgs[i]);
		received_msgs[i] = '-';
	}
    
	receiver_pids[0] = create(receiven_time, 4096, 20, &receiver, 3, 0, count, 5000);
	resume(receiver_pids[0]);
	
	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);
	
}

//test case for myrecvtime and mysendn with  timeout period of 3000ms . Receiver wants to receive greater messages than is sent. As a result, receiver reaches its timeout period
void start_test_case_17(int nsend, int nrecv){
	char sender = 'S';
	char receiver = 'R';

	sent_msgs = msgs;
	received_msgs = rec_msgs;
	int i;
	int count = 5;
	for (i=0;i<count;i++){
		sent_msgs[i] = 'a';
	}
    for (i=0;i<count+4;i++){
		kprintf("%c  Initial received message\n", received_msgs[i]);
		received_msgs[i] = '-';
	}
	receiver_pids[0] = create(receiven_time, 4096, 20, &receiver, 3, 0, count+4, 2000);
	resume(receiver_pids[0]);
	
	sender_pids[0] = create(send_multiple, 4096, 20, &sender, 3, 0, receiver_pids[0], count);
	resume(sender_pids[0]);
	
}

	
	
int main(int argc, char **argv)
{
	/* Place your test cases here - Uncomment each method and comment rest for a particular test case*/

	//start_test_case_1(1,1);

	//start_test_case_2(2,1);

	//start_test_case_3(1,0);
	
	//start_test_case_4(0,1);

	//start_test_case_5(2,2);

	//start_test_case_6(1,1);

	//start_test_case_7(1,1);

	//start_test_case_8(1,1);

	//start_test_case_9(2,1);

	//start_test_case_10(1,1);	
	
	//start_test_case_11(2,1);

	//start_test_case_12(1,1);
	
	//start_test_case_13(1,1);

	//start_test_case_14(2,2);

	//start_test_case_15(1, 1);

	//start_test_case_16(1, 1);
	
	//start_test_case_17(1, 1);
	return OK;
}
