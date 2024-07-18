#include <sys/types.h>	/* For portability */
#include <stddef.h>		/* For definition of offsetof() */
#include "tlpi_hdr.h"

#define SHARED_MESSAGE_QUEUE_KEY 0xffeeddcc /* Key for the shared message queue */

struct requestMsg {	/* Request from client to server */
	long mtype;		/* Used by server to identify messages; will be 1 */
	int seqLen;		/* Length of desired sequence */
	pid_t pid;		/* PID of client */
};

struct responseMsg { /* Response from server to client */
	long mtype;		/* Client's PID */
	int seqNum;		/* Starting sequence number */
};

/* The offsetof() macro in the computation of the size of the 'mtext' part
   of the message structures in order to account for the padding bytes that
   are inserted to satisfy alignment restrictions. */

#define REQ_MSG_SIZE (offsetof(struct requestMsg, pid) - \
						offsetof(struct requestMsg, seqLen) + sizeof(pid_t))

#define RESP_MSG_SIZE (sizeof(int))

/* Types for response messages sent from client to server */

#define TO_SERVER_MSG_TYPE 1
