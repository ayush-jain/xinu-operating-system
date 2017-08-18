/* hpetdispatch.S - hpetdispatch */

#include <icu.s>
		.text
		.globl	hpetdispatch
		.globl	hpethandler

/*------------------------------------------------------------------------
 *  hpetdispatch  -  Dispatch an interrupt to the hpet handler function
 *------------------------------------------------------------------------
 */
hpetdispatch:
		pushal			/* Save general-purpose regs.	*/
		pushfl			/* Save the flags register	*/
		cli			/* Prevent further interrupts	*/
		movb	$EOI,%al	/* Clear the interrupt		*/
		outb	%al,$OCW1_2
		call	hpethandler	/* Call the handler		*/
		popfl			/* Restore the flags register	*/
		popal			/* Restore general-purpose regs.*/
		iret			/* Return from interrupt	*/
