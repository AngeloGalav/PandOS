#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"

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

#endif 