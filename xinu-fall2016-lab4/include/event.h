/* event.h - declarations for Xinu events	*/

/* System events				*/
#define PROCESS_GETMEM_EVENT	1
#define PROCESS_FREEMEM_EVENT	2

/* This range reserved for other system events	*/

#define USER_EVENT_BOUNDARY	32

/* This range is free for user events		*/

#define MAX_EVENTS		64

/* No events are allowed to have values here	*/
