/* find_library_function.c - find_library_function */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  find_library_function  -  Locate a dynamically loaded library function
 *------------------------------------------------------------------------
 */
void*	find_library_function(
	  char		*name		/* Function name to locate	*/
        )
{
	int i;
	kprintf("\nstarting...%d\n", func_size);
	for(i=0;i<func_size;i++){
		kprintf("%s\n", func_info[i].str);
		kprintf("%s\n", name);
		if((strncmp(func_info[i].str, (const char *)name, strlen(name)) == 0) && (strlen(func_info[i].str) == strlen(name))){
			return (void *)func_info[i].add;	
		} 
	}
	return (void*)SYSERR;
}