/* hpet.h - Definitions related to HPET */

#define	HPET_BASE_ADDR	0xFED00000

/* HPET Device control functions	*/
#define	HPET_CTRL_INTR_SET	1
#define	HPET_CTRL_INTR_DEL	2
#define	HPET_CTRL_TTI		3
#define	HPET_CTRL_HOOK_SET	4

/* General Configuration register */
#define	HPET_GC_OE	0x00000001	/* Overall Enable	*/
#define	HPET_GC_LRE	0x00000002	/* Legacy Route Enable	*/

/* General Interrupt Status register */
#define	HPET_GIS_T0	0x00000001	/* Timer 0 interrupt	*/
#define	HPET_GIS_T1	0x00000002	/* Timer 1 interrupt	*/
#define	HPET_GIS_T2	0x00000004	/* Timer 2 interrupt	*/

/* Timer X Config & Capabilities register */
#define	HPET_TXCC_IT	0x00000002	/* Level (edge) interrupt	*/
#define	HPET_TXCC_IE	0x00000004	/* Interrupt Enable		*/

/* Structure of HPET Memory-mapper Registers */
struct	hpet_csreg {
	uint32	gcid_l;		/* General Capabilities & ID lower	*/
	uint32	gcid_u;		/* General Capabilities & ID upper	*/
	uint32	res1[2];	/* Reserved				*/
	uint32	gc;		/* General Configuration		*/
	uint32	res2[3];	/* Reserved				*/
	uint32	gis;		/* General Interrupt Status		*/
	uint32	res3[51];	/* Reserved				*/
	uint32	mcv_l;		/* Main Counter Value lower		*/
	uint32	mcv_u;		/* Main Counter Value upper		*/
	uint32	res4[2];	/* Reserved				*/
	uint32	t0cc_l;		/* Timer 0 Config & Capabilities lower	*/
	uint32	t0cc_u;		/* Timer 0 Config & Capabilities upper	*/
	uint32	t0cv_l;		/* Timer 0 Comparator Value lower	*/
	uint32	t0cv_u;		/* Timer 0 Comparator Value upper	*/
	uint32	res5[4];	/* Reserved				*/
	uint32	t1cc_l;		/* Timer 1 Config & Capabilities lower	*/
	uint32	t1cc_u;		/* Timer 1 Config & Capabilities upper	*/
	uint32	t1cv;		/* Timer 1 Comparator Value		*/
	uint32	res6[5];	/* Reserved				*/
	uint32	t2cc_l;		/* Timer 2 Config & Capabilities lower	*/
	uint32	t2cc_u;		/* Timer 2 Config & Capabilities upper	*/
	uint32	t2cv;		/* Timer 2 Comparator Value		*/
};

/* Structure of an HPET Control Block */
struct	hpetcblk {
	byte	state;		/* State of the device			*/
	struct	dentry *dev;	/* Address in device switch table	*/
	void	*csr;		/* Control & Status register address	*/

	/* TODO: Add additional fields here if necessary */
	void (*hook_func)(void) ;
};

/* Array of HPET Control Blocks */
extern	struct	hpetcblk hpettab[];
