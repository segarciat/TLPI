#include <signal.h> /* struct sigaction, sigemptyset(), SIGCHLD, sigaction(), SA_RESTART */
#include <errno.h>	/* errno, EINTR */
#include <stdlib.h> /* exit(), EXIT_FAILURE */
#include <sys/wait.h> /* waitpid(), WNOHANG */
#include <sys/msg.h>	/* msgrcv(), msgsnd() */
#include <unistd.h> /* fork(), _exit() */
#include <sys/stat.h> /* Permissions, I.E., S_IRUSR, S_IWUSR, S_IRGRP, S_IWGRP */
#include "sysvmq_seqnum.h"

/* Reap children processes */
static void
sigchldHandler(int sig)
{
	int savedErrno = errno;
	/* Wait for any child, ignore exit status, and don't block */
	while (waitpid(-1, NULL, WNOHANG) > 0)
		continue;
	errno = savedErrno;
}

/* Executed in child process to serve a single client */
static void
serveRequest(const struct requestMsg *req, int seqNum, int msgqId)
{
	struct responseMsg resp;
	resp.mtype = req->pid;
	resp.seqNum = seqNum;
	while (msgsnd(msgqId, &resp, RESP_MSG_SIZE, /* flags */ 0) == -1) {
		if (errno == EINTR)
			continue;			/* Restart if interrupted */
		_exit(EXIT_FAILURE);
	}
}

int
main(int argc, char *argv[])
{
	struct sigaction sa;
	ssize_t msgLen;
	pid_t pid;
	int seqNum, sharedMsgqId;
	struct requestMsg req;
	
	seqNum = 0;		/* This is our 'service' */

	/* Create message queue */
	sharedMsgqId = msgget(SHARED_MESSAGE_QUEUE_KEY, IPC_CREAT | IPC_EXCL |
									S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	/* Install SIGCHLD handler to reap children processes that are forked to handle requests */
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigchldHandler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
		errExit("sigaction - SIGCHLD handler");

	/* Read a request and handle in a separate child process */
	for (;;) {
		msgLen = msgrcv(sharedMsgqId, &req, REQ_MSG_SIZE, TO_SERVER_MSG_TYPE, /* Flags */ 0);
		if (msgLen == -1) {
			if (errno == EINTR)			/* Interrupted by SIGCHLD handler */
				continue;				/* Restart, since SA_RESTART does not apply to msgrcv */
			errMsg("msgrcv failed");	/* Some other error */
			break;						/* Terminate loop */
		}

		pid = fork();					/* Create child to handle request */
		if (pid == -1) {
			errMsg("Failed to fork child to handle client request");
			break;
		}

		if (pid == 0) {					/* Child handles request and exits */
			serveRequest(&req, seqNum, sharedMsgqId);
			_exit(EXIT_SUCCESS);
		}

		seqNum += req.seqLen;			/* Increment sequence number */
	}
	exit(EXIT_FAILURE);
}
