/*  prod_cons.h  - definitions for producer and consumer		*/

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/*                                                                      */
/*  DEFINE ALL NECESSARY VARIABLES IN prod_cons.c                       */
/************************************************************************/

/* Globals variables */
#define NPRODUCERS	10		/* The number of producers	*/
#define NCONSUMERS	10		/* The number of consumers	*/
#define BUFFERSIZE	200		/* The size of the shared	*/
					/*    buffer			*/

/* Tags for each producer						*/
char producer_tags[NPRODUCERS] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };	

/* Sleep time for each producer	(in ms)					*/
int producer_sleep_times[NPRODUCERS] = { 10, 2000, 3000, 6000, 500, 250, 400, 50, 80, 100 };

/* Production counts, the number of items produced by each producer	*/
int producer_counts[NPRODUCERS] = { 5, 1, 2, 1, 56, 1, 30, 20, 12, 1 };


/* Tags for each consumer						*/
char consumer_tags[NCONSUMERS] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };

/* Sleep times for each consumer (in ms)				*/
int consumer_sleep_times[NCONSUMERS] = { 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000 };

/* Cosumption counts, the number of items consumed by each consumer	*/
int consumer_counts[NCONSUMERS] = { 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 };

/* Declarations for functions used by producer/consumer simulation	*/
void start_prod_con(void);
void stop_prod_con(void);
void print_report(void);
