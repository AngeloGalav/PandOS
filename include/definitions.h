#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"

/* System call codes */
#define	CREATETHREAD	1	/* create thread */
#define	TERMINATETHREAD	2	/* terminate thread */
#define	PASSERN			3	/* P a semaphore */
#define	VERHOGEN		4	/* V a semaphore */
#define	WAITIO			5	/* delay on a io semaphore */
#define	GETCPUTIME		6	/* get cpu time used to date */
#define	WAITCLOCK		7	/* delay on the clock semaphore */
#define	GETSPTPTR		8	/* return support structure ptr. */

/* Devices constants */
#define TRANSMITTED	5
#define ACK	1
#define PRINTCHR	2


/* Total number of device semaphores in PandOS */
#define SEMAPHORE_QTY 49

/* Interval timer semaphore index */
#define INTERVALTIMER_INDEX 48

/* Timer converter to get the right time */
#define TIMERVALUE(T)  ((T) * (*((cpu_t *) TIMESCALEADDR)))

/* Value to assign to ExeCode of cause register if entering a syscall in usermode */
#define RESVINSTR  10

/* Base addr for interrupting device area, which tells us which device has a pending interrupt*/
#define IDEVBITMAP 0x10000040 

/* Current TOD value */
#define CURRENT_TOD ((*((memaddr *) TODLOADDR)) / (*((cpu_t *) TIMESCALEADDR)))

/* Creates a variable that contains the process state stored in the biosdatapage before an interrupt */
#define GET_BDP_STATUS(T) state_t* T = (state_t*) BIOSDATAPAGE

/* Start of the device register memory area */
#define DEVREG 0x10000054

/* Set the VPN of a TLB/PageTable entry */
#define SET_VPN(U, x) ( U |= (memaddr)(x) << VPNSHIFT) 

/* Set the ASID of a TLB/PageTable entry */
#define SET_ASID(U, x) ( U |= (memaddr) (x) << ASIDSHIFT )

/* Set D and G bit of EntryLo of a TLB/PageTable entry */
#define SET_D_AND_G(U) ( U |=  DIRTYON | GLOBALON)

/* Extract excCode from Cause register */
#define GET_EXEC_CODE(S) BitExcractor(S, GETEXECCODE, CAUSESHIFT) 

#endif 