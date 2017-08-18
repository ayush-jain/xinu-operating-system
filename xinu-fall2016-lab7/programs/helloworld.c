#include <xinu.h>

int a = 10;	
int32 main(int32 argc, char* argv[])
{
	kprintf("Hello World...%d\n", a);
	return 0;
}
