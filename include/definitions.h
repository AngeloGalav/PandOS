#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"
#include <umps3/umps/regdef.h>
#include "umps3/umps/cp0.h"

/* System call codes */
#define CREATETHREAD 1    /* create thread */
#define TERMINATETHREAD 2 /* terminate thread */
#define PASSERN 3         /* P a semaphore */
#define VERHOGEN 4        /* V a semaphore */
#define WAITIO 5          /* delay on a io semaphore */
#define GETCPUTIME 6      /* get cpu time used to date */
#define WAITCLOCK 7       /* delay on the clock semaphore */
#define GETSPTPTR 8       /* return support structure ptr. */

/* Devices constants */
#define TRANSMITTED 5
#define ACK 1
#define PRINTCHR 2
#define EOF '\0'
#define EOL '\n'
#define READBLK 2

/* Support Layer Devices Lines (!= device line in nucleus)*/
#define PRINTER 0
#define WRITETERM 1
#define READTERM 2

/* EntryLo useful constants */
#define PFNSHIFT 12
#define VALID_BIT_POS 9

/* Number of support semaphore types */
#define SUPP_SEM_N 3

/* Total number of kernel device semaphores in PandOS */
#define SEMAPHORE_QTY 49

/* Interval timer semaphore index */
#define INTERVALTIMER_INDEX 48

/* Timer converter to get the right time */
#define TIMERVALUE(T) ((T) * (*((cpu_t *)TIMESCALEADDR)))

/* Value to assign to ExeCode of cause register if entering a syscall in usermode */
#define RESVINSTR 10

/* Base addr for interrupting device area, which tells us which device has a pending interrupt*/
#define IDEVBITMAP 0x10000040

/* Current TOD value */
#define CURRENT_TOD ((*((memaddr *)TODLOADDR)) / (*((cpu_t *)TIMESCALEADDR)))

/* Creates a variable that contains the process state stored in the BIOSDATAPAGE before an interrupt */
#define GET_BDP_STATUS(T) state_t *T = (state_t *)BIOSDATAPAGE

/* Start of the device register memory area */
#define DEVREG 0x10000054

/* Set the VPN of a TLB/PageTable entry */
#define SET_VPN(U, x) (U |= (memaddr)(x) << VPNSHIFT)

/* Set the ASID of a TLB/PageTable entry */
#define SET_ASID(U, x) (U |= (memaddr)(x) << ASIDSHIFT)

/* Set D and G bit of EntryLo of a TLB/PageTable entry */
#define SET_D(U) (U |= DIRTYON)

/* Extract excCode from Cause register */
#define GET_EXEC_CODE(S) BitExtractor(S, GETEXECCODE, CAUSESHIFT)

/* Unset pos bit of a T value */
#define UNSET_BIT(T, pos) (T &= ~(1 << pos))

/* Macro which disables the interrupts by modifying the status bitmap */
#define DISABLE_INTERRUPTS_COMMAND setSTATUS(getSTATUS() & DISABLEINTS)

/* Macro which enables the interrupts by modifying the status bitmap */
#define ENABLE_INTERRUPTS_COMMAND setSTATUS(getSTATUS() | IECON);

/* Swap pool starting address */
#define POOLSTART 0x20020000

/* Page Table Starting Address */
#define PAGETBLSTART 0x80000000

#define GET_DEV_ADDR(line, device) (devreg_t*) (DEVREG + (((line) - 3) * 0x80) + ((device) * 0x10))

#endif