/* alarm.h - Definitions related to the Alarm System */

#define	NALARMS	20	/* Max. no. of registered alarms	*/




/* Alarm Queue structure declarations, constants, and inline functions	*/

/* Default # of queue entries: 1 per palarm plus 2 for alarm queue*/
#ifndef NAENT
#define NAENT	(NALARMS + 2)
#endif

#define	EMPTY	(-1)		/* Null value for qnext or qprev index	*/
#define	MAXKEY	0x7FFFFFFF	/* Max key that can be stored in queue	*/
#define	MINKEY	0x80000000	/* Min key that can be stored in queue	*/

struct	aentry	{		/* One per process plus two per list	*/
	uint32	akey;		/* Key on which the queue is ordered	*/
	qid16	anext;		/* Index of next process or tail	*/
	qid16	aprev;		/* Index of previous process or head	*/
};

extern	struct aentry	alarmtab[];

/* Inline queue manipulation functions */

#define	alarmqhead(q)	(q)
#define	alarmqtail(q)	((q) + 1)
#define	firstaid(q)	(alarmtab[alarmqhead(q)].anext)
#define	lastaid(q)	(alarmtab[alarmqtail(q)].aprev)
#define	isempty_aq(q)	(firstaid(q) >= NALARMS)
#define	nonempty_aq(q)	(firstaid(q) <  NALARMS)
#define	firstakey(q)	(alarmtab[firstaid(q)].akey)
#define	lastakey(q)	(alarmtab[ lastaid(q)].akey)

/* Inline to check queue id assumes interrupts are disabled */

struct alarm_info{
	void (*callbackfn)(int32 arg);
	int32 arg;	
};

extern struct alarm_info infotab[];
extern int alarmqid;
extern int alarmqsize;

#define	isbadaid(x)	(((int32)(x) < 0) || (int32)(x) >= NALARMS)
