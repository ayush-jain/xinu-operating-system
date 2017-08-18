/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/************************************************************************/

void callbackfn(int32 arg) {
    kprintf("Callback with argument %d\n", arg);
}

void callbackfn1(int32 arg) {
    kprintf("New Callback with argument %d\n", arg);
}

void traverse(){
	int32 next = alarmqhead(alarmqid);
	int32 val;
	kprintf("Traversing list...size %d..\n", alarmqsize);
	while(next != alarmqtail(alarmqid)){
		val = alarmtab[next].akey;
		kprintf("%u...\n", val);		
		next = alarmtab[next].anext;
	}			
}

//Checking registeration of 3 alarms along with initialization, hpethandler functioning along with alarm_trigger (hook function) correctness by seeing output of debugging statements
void start_test_case_1(){

	int32  ret1, ret2, ret3;

    ret1 = alarm_register(5000, callbackfn, 4);
    kprintf("return is %d\n", ret1);
	traverse();

	ret2 = alarm_register(5000, callbackfn, 5);
	traverse();		
	kprintf("new return is %d\n", ret2);
    
    ret3 = alarm_register(3000, callbackfn, 8);
	traverse();
	
	sleep(3);	
	kprintf("new return is %d\n", ret3);

}

// Single alarm registration and deregistration
void start_test_case_2(){

	int32  ret1;
	ret1 = alarm_register(5000, callbackfn, 4);
    kprintf("return is %d\n", ret1);
	traverse();

	
	int status = alarm_deregister(ret1);
	kprintf("status is %d\n", status);
	traverse();
}

// Multiple alarm registration and deregistration at various instances of time
// Playing with time values in registration and alarm id values in deregistration here to see insertion/removal at head,tail and middle of list
void start_test_case_3(){

	int32  ret1, ret2, ret3, ret4;
    ret1 = alarm_register(5000, callbackfn, 4);
    kprintf("return is %d\n", ret1);
	traverse();

	ret2 = alarm_register(5000, callbackfn, 5);
	traverse();		
	kprintf("new return is %d\n", ret2);
    
    ret3 = alarm_register(3000, callbackfn1, 8);
	kprintf("new return is %d\n", ret3);
	traverse();
	
	sleep(3);	
	

	int status = alarm_deregister(ret2);
	kprintf("status is %d\n", status);
	traverse();

	ret4 = alarm_register(6000, callbackfn, 10);
	kprintf("new return is %d\n", ret4);
	traverse();

	sleep(7);
	traverse();
	
}

//invalid deregistration
void start_test_case_4(){

	int32  ret2 = 0;

	int status = alarm_deregister(ret2);
	kprintf("status is %d\n", status);
	traverse();
}

//invalid registration by trying to register 21st alarm
void start_test_case_5(){
	int i;
	int32  status;
	for(i=0;i<NALARMS+1;i++){
    	status = alarm_register(5000+(1000*i), callbackfn, i);
    	kprintf("return is %d\n", status);
    }

    status = alarm_deregister(17);
	kprintf("status is %d\n", status);
	traverse();

    status = alarm_register(5000, callbackfn, 25);
    kprintf("return is %d\n", status);	
}

//trying to deregister after alarm interrupt has been handled for 3rd alarm
void start_test_case_6(){

	int32  ret1, ret2, ret3, ret4;
    ret1 = alarm_register(5000, callbackfn, 4);
    kprintf("return is %d\n", ret1);
	traverse();

	ret2 = alarm_register(5000, callbackfn, 5);
	traverse();		
	kprintf("new return is %d\n", ret2);
    
    ret3 = alarm_register(3000, callbackfn, 8);
	kprintf("new return is %d\n", ret3);
	traverse();
	
	sleep(3);	
	

	int status = alarm_deregister(ret3);
	kprintf("status is %d\n", status);
	traverse();

	ret4 = alarm_register(6000, callbackfn, 10);
	kprintf("new return is %d\n", ret4);
	traverse();

	sleep(7);
	traverse();

	
}

process	main(void)
{
	/* Place your test cases here */
	//Uncomment each specific test case below for its execution. Comments written along each test case above 

	//start_test_case_1();

	//start_test_case_2();

	//start_test_case_3();

	//start_test_case_4();

	//start_test_case_5();

	//start_test_case_6();

	return OK;

}
