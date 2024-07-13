# Exercise 44.3

The server in Listing 44-7 (`fifo_seqnum_server.c`) always starts assigning sequence number
from 0 each time it is started. Modify the program to use a backup file that is updated each
time a sequence number is assigned. (The *open()* `O_SYNC` flag described in Section 4.3.1
may be useful.) At startup, the program should check for the existence of this file, and if
it is preset, use the value it contains to initialize the sequence number. If the backup
file can't be found on startup, the program should create a new file and start assigning
sequence numbers beginning at 0. (An alternative to this technique would be to use memory-mapped
files, described in Chaper 49.)

## Solution

I began by opening the backup file for the sequence number, creating if it did not exist:

```c
	/* Create sequence number backup file */
	seqnumBackupFd = open(SEQNUM_BACKUP_FILE, O_RDWR | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
	if (seqnumBackupFd == -1)
		errExit("open seqnum backup");
	
	bytesRead = read(seqnumBackupFd, seqnumBackupBuf, SEQNUM_TEXT_LEN - 1);
	if (bytesRead == -1)
		errExit("read seqnum backup");
	if (bytesRead == 0) { /* No previous sequence number in backup file */
		printf("Starting from 0\n");
		seqNum = 0;
	}
	else {
		/* Read starting seqnum */
		seqnumBackupBuf[bytesRead] = '\0';
		seqNum = getInt(seqnumBackupBuf, GN_NONNEG, "seqnum-backup");
		printf("read starting seqnum: %d\n", seqNum);
	}
```

If the read returns EOF, perhaps we've just opened the file for the first time or we have not yet
assigned sequence numbers, then the sequence number starts at 0. Otherwise, I use `getInt()`
to parse the starting sequence number. I do not close the backup file, since I overwrite its
contents each time I assign a sequence number to a client at the end of the main loop:

```c
        seqNum += req.seqLen;           /* Update our sequence number */
		printf("next seqNum: %d\n", seqNum);
		/* Update starting sequence number in backup file */
		memset(seqnumBackupBuf, 0, SEQNUM_TEXT_LEN);
		int seqnumLen = snprintf(seqnumBackupBuf, SEQNUM_TEXT_LEN, "%d", seqNum);
		if (lseek(seqnumBackupFd, 0, SEEK_SET) == -1)
			errExit("lseek failed: failed to backup seqNum %d\n", seqNum);
		if (write(seqnumBackupFd, seqnumBackupBuf, seqnumLen) != seqnumLen)
			errExit("write failed: unable to backup seqNum %d\n", seqNum);
```

After the sequence number is updated, I overwrote the file's contents with the new sequence number
prior to servicing the next client.

The output looks as follows on startup, and each time a client is serviced:

```bash
# Compiles server by default
make

# Compile client
make fifo_seqnum_client

# Run server as a background job
./fifo_seqnum_server &

# Request sequence numbers from server
./fifo_seqnum_client
```

With the `printf()` statements I added on the server, the output looks as follows:

```
read starting seqnum: 55
next seqNum: 56
next seqNum: 57
next seqNum: 58
next seqNum: 59
```
