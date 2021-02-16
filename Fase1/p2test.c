/*********************************P1TEST.C*******************************
 *
 *	Test program for the modules ASL and pcbQueues (phase 1).
 *
 *	Produces progress messages on terminal 0 in addition
 *		to the array ``okbuf[]''
 *		Error messages will also appear on terminal 0 in
 *		addition to the array ``errbuf[]''.
 *
 *		Aborts as soon as an error is detected.
 *
 *      Modified by Michael Goldweber on May 15, 2004
 */

#include "pandos_const.h"
#include "pandos_types.h" // prima aveva i simboli > <


#include "umps3/umps/libumps.h"
//#include <pcb.h>
//#include <asl.h>
//#include "pcb_test.h"
#include "testami.h"
#include "stdio.h"


#define MAXINT 0xFFFFFFFF
#define HIDDEN static
#define TRUE            1
#define FALSE           0

#define	MAXSEM	MAXPROC

char okbuf[2048];			/* sequence of progress messages */
char errbuf[128];			/* contains reason for failing */
char msgbuf[128];			/* nonrecoverable error message before shut down */
int sem[MAXSEM];
int onesem;
pcb_t	*procp[MAXPROC], *p, *qa, *q, *firstproc, *lastproc, *midproc;
char *mp = okbuf;


#define TRANSMITTED	5
#define ACK	1
#define PRINTCHR	2
#define CHAROFFSET	8
#define STATUSMASK	0xFF
#define	TERM0ADDR	0x10000254

typedef unsigned int devreg;

/* This function returns the terminal transmitter status value given its address */
devreg termstat(memaddr * stataddr) {
	return((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns TRUE if
 * print was successful, FALSE if not   */
unsigned int termprint(char * str, unsigned int term) {
	memaddr * statusp;
	memaddr * commandp;
	devreg stat;
	devreg cmd;
	unsigned int error = FALSE;

	if (term < DEVPERINT) {
		/* terminal is correct */
		/* compute device register field addresses */
		statusp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
		commandp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));

		/* test device status */
		stat = termstat(statusp);
		if (stat == READY || stat == TRANSMITTED) {
			/* device is available */

			/* print cycle */
			while (*str != EOS && !error) {
				cmd = (*str << CHAROFFSET) | PRINTCHR;
				*commandp = cmd;

				/* busy waiting */
				stat = termstat(statusp);
				while (stat == BUSY)
					 stat = termstat(statusp);

				/* end of wait */
				if (stat != TRANSMITTED)
					error = TRUE;
				else
					/* move to next char */
					str++;
			}
		}
		else
			/* device is not available */
			error = TRUE;
	}
	else
		/* wrong terminal device number */
		error = TRUE;

	return (!error);
}


/* This function placess the specified character string in okbuf and
*	causes the string to be written out to terminal0 */
void addokbuf(char *strp) {
	char *tstrp = strp;
	while ((*mp++ = *strp++) != '\0')
		;
	mp--;
	termprint(tstrp, 0);
}


/* This function placess the specified character string in errbuf and
*	causes the string to be written out to terminal0.  After this is done
*	the system shuts down with a panic message */
void adderrbuf(char *strp) {
	char *ep = errbuf;
	char *tstrp = strp;

	while ((*ep++ = *strp++) != '\0');

	termprint(tstrp, 0);

	//PANIC();
}



void main() {
	int i;

	initPcbs();
	printf("Initialized process control blocks   \n");

	/* Check allocProc */
	for (i = 0; i < MAXPROC; i++) {
		if ((procp[i] = allocPcb()) == NULL)
			printf("allocPcb: unexpected NULL   ");
	}

    printf("over %d\n", i);

	if (allocPcb() != NULL) {
		printf("allocPcb: allocated more than MAXPROC entries   ");
	}
	printf("allocPcb ok   \n");

	/* return the last 10 entries back to free list */
	for (i = 10; i < MAXPROC; i++)
		freePcb(procp[i]);
	printf("freed 10 entries   \n");

	/* create a 10-element process queue */
	qa = NULL;

	if (!emptyProcQ(qa))
        {
            printf("emptyProcQ: unexpected FALSE   ");
        }
	printf("Inserting...   \n");
	for (i = 0; i < 10; i++) {
		if ((q = allocPcb()) == NULL)
			printf("allocPcb: unexpected NULL while insert   ");
		switch (i) {
		case 0:
			firstproc = q;
			break;
		case 5:
			midproc = q;
			break;
		case 9:
			lastproc = q;
			break;
		default:
			break;
		}
		insertProcQ(&qa, q);
	}
	printf("inserted 10 elements   \n");

	if (emptyProcQ(qa)) printf("emptyProcQ: unexpected TRUE"   );

	/* Check outProc and headProc */
	if (headProcQ(qa) != firstproc)
		printf("headProcQ failed   ");
	q = outProcQ(&qa, firstproc);
	if (q == NULL || q != firstproc)
		printf("outProcQ failed on first entry   ");
	freePcb(q);
	q = outProcQ(&qa, midproc);
	if (q == NULL || q != midproc)
		printf("outProcQ failed on middle entry   ");
	freePcb(q);
	if (outProcQ(&qa, procp[0]) != NULL)
		printf("outProcQ failed on nonexistent entry   ");
	printf("outProcQ ok   \n");

	/* Check if removeProc and insertProc remove in the correct order */
	printf("Removing...   \n");
	for (i = 0; i < 8; i++) {
		if ((q = removeProcQ(&qa)) == NULL)
			printf("removeProcQ: unexpected NULL   ");
		freePcb(q);
	}
	if (q != lastproc)
		printf("removeProcQ: failed on last entry   ");
	if (removeProcQ(&qa) != NULL)
		printf("removeProcQ: removes too many entries   ");

        if (!emptyProcQ(qa))
                printf("emptyProcQ: unexpected FALSE   ");

	printf("insertProcQ, removeProcQ and emptyProcQ ok   \n");
	printf("process queues module ok      \n");

	printf("checking process trees...\n");

	if (!emptyChild(procp[2]))
	  printf("emptyChild: unexpected FALSE   ");

	/* make procp[1] through procp[9] children of procp[0] */
	printf("Inserting...   \n");
	for (i = 1; i < 10; i++) {
		insertChild(procp[0], procp[i]);
	}
	printf("Inserted 9 children   \n");

	if (emptyChild(procp[0]))
	  printf("emptyChild: unexpected TRUE   ");

	/* Check outChild */
	q = outChild(procp[1]);
	if (q == NULL || q != procp[1])
		printf("outChild failed on first child   ");
	q = outChild(procp[4]);
	if (q == NULL || q != procp[4])
		printf("outChild failed on middle child   ");
	if (outChild(procp[0]) != NULL)
		printf("outChild failed on nonexistent child   ");
	printf("outChild ok   \n");

	/* Check removeChild */
	printf("Removing...   \n");
	for (i = 0; i < 7; i++) {
		if ((q = removeChild(procp[0])) == NULL)
			printf("removeChild: unexpected NULL   ");
	}
	if (removeChild(procp[0]) != NULL)
	  printf("removeChild: removes too many children   ");

	if (!emptyChild(procp[0]))
	    printf("emptyChild: unexpected FALSE   ");

	printf("insertChild, removeChild and emptyChild ok   \n");
	printf("process tree module ok      \n");

	for (i = 0; i < 10; i++)
		freePcb(procp[i]);


	/* check ASL */
	initASL();
	printf("Initialized active semaphore list   \n");

	/* check removeBlocked and insertBlocked */
	printf("insertBlocked test #1 started  \n");
	for (i = 10; i < MAXPROC; i++) {
		procp[i] = allocPcb();
		if (insertBlocked(&sem[i], procp[i]))
			printf("insertBlocked(1): unexpected TRUE   ");
	}
	printf("insertBlocked test #2 started  \n");
	for (i = 0; i < 10; i++) {
		procp[i] = allocPcb();
		if (insertBlocked(&sem[i], procp[i]))
			printf("insertBlocked(2): unexpected TRUE   ");
	}

	/* check if semaphore descriptors are returned to free list */
	p = removeBlocked(&sem[11]);
	if (insertBlocked(&sem[11],p))
		printf("removeBlocked: fails to return to free list   ");

	if (insertBlocked(&onesem, procp[9]) == FALSE)
		printf("insertBlocked: inserted more than MAXPROC   ");

	printf("removeBlocked test started   \n");
	for (i = 10; i< MAXPROC; i++) {
		q = removeBlocked(&sem[i]);
		if (q == NULL)
			printf("removeBlocked: wouldn't remove   ");
		if (q != procp[i])
			printf("removeBlocked: removed wrong element   ");
		if (insertBlocked(&sem[i-10], q))
			printf("insertBlocked(3): unexpected TRUE   ");
	}
	if (removeBlocked(&sem[11]) != NULL)
		printf("removeBlocked: removed nonexistent blocked proc   ");
	printf("insertBlocked and removeBlocked ok   \n");

	if (headBlocked(&sem[11]) != NULL)
		printf("headBlocked: nonNULL for a nonexistent queue   ");
	if ((q = headBlocked(&sem[9])) == NULL)
		printf("headBlocked(1): NULL for an existent queue   ");
	if (q != procp[9])
		printf("headBlocked(1): wrong process returned   ");
	p = outBlocked(q);
	if (p != q)
		printf("outBlocked(1): couldn't remove from valid queue   ");
	q = headBlocked(&sem[9]);
	if (q == NULL)
		printf("headBlocked(2): NULL for an existent queue   ");
	if (q != procp[19])
		printf("headBlocked(2): wrong process returned   ");
	p = outBlocked(q);
	if (p != q)
		printf("outBlocked(2): couldn't remove from valid queue   ");
	p = outBlocked(q);
	if (p != NULL)
		printf("outBlocked: removed same process twice.");
	if (headBlocked(&sem[9]) != NULL)
		printf("out/headBlocked: unexpected nonempty queue   ");
	printf("headBlocked and outBlocked ok   \n");
	printf("ASL module ok   \n");
	printf("So Long and Thanks for All the Fish\n");

}

