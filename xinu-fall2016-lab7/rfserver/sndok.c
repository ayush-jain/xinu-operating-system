/* sndok.c - sndok */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

/*------------------------------------------------------------------------
 * sndok - send an "OK" response (status is zero)
 *------------------------------------------------------------------------
 */
void	sndok (
	 struct	rf_msg_hdr *reqptr,	/* ptr to request		*/
	 struct	rf_msg_hdr *resptr,	/* ptr to response		*/
	 int	len			/* length of header to copy	*/
					/*    into response		*/
	)
{
	int	i;			/* counts bytes in header	*/
	char	*from, *to;		/* pointers used during copy	*/
	int	retval;

	/* copy header to from request to response */

	from = (char *)reqptr;
	to =   (char *)resptr;
	for (i=0; i<sizeof(struct rf_msg_hdr); i++) {
		*to++ = *from++;
	}

	/* Set bit to indicate response */

	resptr->rf_type = htons(ntohs(resptr->rf_type) | RF_MSG_RESPONSE);

	/* Set status to indicate OK (should be zero anyway) */

	resptr->rf_status = htons(0);

	/* Return response to source */

	retval = sendto(sock, (const void *)resptr, len, 0,
		(struct sockaddr *)&senderip, addrlen);

	return;
}
