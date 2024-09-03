#ifndef _SELECT_SVMQ_H
#define _SELECT_SVMQ_H

#define MAX_MQTEXT 1024
struct mqbuf {
    long mtype;                 /* Message type */
    char mqtext[MAX_MQTEXT];    /* Message body */
};

#endif
