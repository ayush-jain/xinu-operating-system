/* rfserver.c - main */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>

#include "xinudefs.h"
#include "rfilesys.h"
#include "rfserver.h"

struct	fentry ofiles[MAXFILES];	/* open file table		*/
int	findex;				/* index of file in ofiles	*/
struct	fentry	*fptr;			/* pointer to entry in ofiles	*/

int	fnext = 0;			/* next entry in ofiles to test	*/

int	sock;				/* UDP socket used to receive	*/
					/*   requests & send responses	*/
struct	sockaddr_in senderip;		/* the sender's address taken	*/
					/*   the current UDP packet	*/
int	addrlen;			/* size of the above address	*/

extern	void*	memset(void *, int, size_t);
/*------------------------------------------------------------------------
 * main - implement a Xinu remote file server running on a Unix machine
 *------------------------------------------------------------------------
 */
int	main (int argc, char *argv[]) 
{
	char	inbuf[MAXMSG];		/* space big enough for largest	*/
					/*   message that can arrive	*/ 
	char	outbuf[MAXMSG];		/* space big enough for largest	*/
					/*   response that can be sent	*/
	struct	rf_msg_hdr *mptr;	/* ptr to incoming request	*/
	struct	rf_msg_hdr *rptr;	/* ptr to outgoing response	*/
	struct	protoent *pptr;		/* ptr to protocol information	*/

	int	seq = 0;		/* sequence number in previous	*/
					/*   message			*/
	int	thisseq;		/* sequence in current packet	*/
	int	i, j;			/* general loop index variables	*/
	int	n;			/* number of bytes in UDP read	*/
	int	msgtyp;			/* message type in packet	*/
	int	err;			/* non-zero when error detected	*/
	int	retval;			/* return value			*/
	char	*to, *from;		/* used during name copy	*/
	int	len;			/* length of name		*/

	/* initialize table that records open files */

	for (i=0; i<MAXFILES; i++) {
		ofiles[i].desc = -1;
		ofiles[i].dirptr = NULL;
		for (j=0; j<RF_NAMLEN; j++) {
			ofiles[i].name[j] = NULLCH;
		}
	}

	memset((void *)&senderip, 0, (size_t) sizeof(senderip));
	senderip.sin_family = AF_INET;
	senderip.sin_addr.s_addr = INADDR_ANY;
	senderip.sin_port=htons((unsigned short)RF_SERVER_PORT);
	pptr = getprotobyname("udp");
	sock = socket(PF_INET, SOCK_DGRAM, pptr->p_proto);
	if (sock < 0) {
		printf("Error opening socket");
		exit(1);
	}

	/* Bind the socket */

	retval = bind( sock, (struct sockaddr *)&senderip,
			sizeof(senderip) );
	if (retval < 0) {
		printf("Error: cannot bind to UDP port %d\n\n",
				RF_SERVER_PORT);
		exit(1);
	} else {
		printf("Server is using UDP port %d\n\n",
				RF_SERVER_PORT);
	}

	mptr = (struct rf_msg_hdr *)inbuf;
	rptr = (struct rf_msg_hdr *)outbuf;
	addrlen = sizeof(senderip);

	while (1) {

		n = recvfrom(sock, &inbuf, MAXMSG, 0,
			(struct sockaddr *) &senderip, &addrlen);

		/* ignore if message is too small or an error occurrred	*/

		if ( n < sizeof(struct rf_msg_hdr) ) {
			continue;
		}

		/* ignore if message type is invalid */

		msgtyp = ntohs(mptr->rf_type & 0xffff);
		if ( (msgtyp < RF_MIN_REQ) || (msgtyp > RF_MAX_REQ) ) {
			continue;
		}

		/* if incoming sequence is 1, reset local seq */

		thisseq = htonl(mptr->rf_seq);
		if (thisseq == 1) {		
			seq = 0;
		}

		/* ignore if sequence in packet is non-zero and is less	*/
		/*	than the server's sequence number		*/

		if ( (thisseq != 0) && (thisseq < seq) ) {
			continue;
		}

		seq = thisseq;		/* record sequence		*/

		/* Ignore if name is too long */


		for (len=0; len<RF_NAMLEN; len++) {
			if (mptr->rf_name[len] == NULLCH) {
				break;
			}
		}
		if (len >= RF_NAMLEN) {
			snderr(mptr, rptr, sizeof(struct rf_msg_hdr) );
			continue;
		}

		/* Ignore if name contains starts with slash */

		if (mptr->rf_name[0]=='/') {
			snderr(mptr, rptr, sizeof(struct rf_msg_hdr) );
			continue;
		}

		/* Ignore if name contains substring ".." */

		err = 0;
		for (i=0; i<len; i++) {
			if (mptr->rf_name[i] == NULLCH) {
				break;
			} else if ( (mptr->rf_name[i] == '.') &&
			     (mptr->rf_name[i+1] == '.') ) {
				err = 1;
				break;
			}
		}
		if (err > 0) {
			snderr(mptr, rptr, sizeof(struct rf_msg_hdr) );
			continue;
		}

		/* see if we already have the file opened */

		for (findex=0; findex<MAXFILES; findex++) {
			fptr = &ofiles[findex];
			if ((fptr->desc < 0) && (fptr->dirptr == NULL)) {
				continue;
			}
			if (strncmp(mptr->rf_name,fptr->name,RF_NAMLEN)
				== 0) {
				break;
			}
		}
		if (findex >= MAXFILES) {
			findex = -1;	/* file not found */
		}

		/* process message */

		switch (msgtyp) {

		case RF_MSG_RREQ:
			rsread(	  (struct rf_msg_rreq *)mptr,
				  (struct rf_msg_rres *)rptr );
			break;

		case RF_MSG_WREQ:
			rswrite(  (struct rf_msg_wreq *)mptr,
				  (struct rf_msg_wres *)rptr );
			break;

		case RF_MSG_OREQ:
			rsopen (  (struct rf_msg_oreq *)mptr,
				  (struct rf_msg_ores *)rptr );
			break;

		case RF_MSG_DREQ:
			rsdelete( (struct rf_msg_dreq *)mptr,
				  (struct rf_msg_dres *)rptr );
			break;

		case RF_MSG_TREQ:
			rstrunc(  (struct rf_msg_treq *)mptr,
				  (struct rf_msg_tres *)rptr );
			break;

		case RF_MSG_SREQ:
			rsstat(	  (struct rf_msg_sreq *)mptr,
				  (struct rf_msg_sres *)rptr );
			break;

		case RF_MSG_MREQ:
			rsmkdir(  (struct rf_msg_mreq *)mptr,
				  (struct rf_msg_mres *)rptr );
			break;

		case RF_MSG_XREQ:
			rsrmdir(  (struct rf_msg_xreq *)mptr,
				  (struct rf_msg_xres *)rptr );
			break;

		case RF_MSG_CREQ:
			rsclose(  (struct rf_msg_creq *)mptr,
				  (struct rf_msg_cres *)rptr );
		}
	}
}
