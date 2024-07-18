#include "sysvmq_seqnum.h"
#include <stdlib.h> /* exit(), EXIT_SUCCESS */
#include <stdio.h> /* printf() */
#include <sys/types.h> /* For portability */
#include <sys/msg.h> /* msgget() */
#include <unistd.h> /* getpid() */
#include <string.h> /* strcmp() */
#include <sys/stat.h> /* Permission flags: S_IRUSR, S_IWUSR */
#include <errno.h> /* errno, EINTR */

int
main(int argc, char *argv[])
{
	int sharedMsgqId;
	struct requestMsg req;
	struct responseMsg resp;
	pid_t pid;

	if (argc > 1 && strcmp(argv[1], "--help") == 0)
        usageErr("%s [seq-len]\n", argv[0]);

	/* Open System V message queue */
	sharedMsgqId = msgget(SHARED_MESSAGE_QUEUE_KEY, S_IRUSR | S_IWUSR);
	if (sharedMsgqId ==-1)
		errExit("msgget - failed to open System V Message Queue");

	/* Build request */
	pid = getpid();
	req.pid = pid;
	req.seqLen = (argc > 1) ? getInt(argv[1], GN_GT_0, "seq-len") : 1;
	req.mtype = TO_SERVER_MSG_TYPE;

	/* Send request to server */
	while (msgsnd(sharedMsgqId, &req, REQ_MSG_SIZE, 0) == -1) {
		if (errno == EINTR)
			continue;
		errExit("msgsend - Failed to send message on queue");
	}

	/* Read message from queue */
	while (msgrcv(sharedMsgqId, &resp, RESP_MSG_SIZE, pid, 0) == -1) {
		if (errno == EINTR)
			continue;
		errExit("msgrcv - Failed to receive message from queue");
	}

    printf("%d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}
