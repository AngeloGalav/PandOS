#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

/* Total number of device semaphores in PandOS */
#define SEMAPHORE_QTY 49

/* Timer converter to get the right time */
#define TIMERVALUE(T)  ((T) * (*((cpu_t *) TIMESCALEADDR)))

/* Value to assign to ExeCode of cause register if entering a syscall in usermode */
#define RESERVEDINSTRUCTION 10

/* Base addr for interrupting device area, which tells us which device has a pending interrupt*/
#define IDEVBITMAP 0x10000040 

/* Current TOD value */
#define CURRENT_TOD ((*((memaddr *) TODLOADDR)) / (*((cpu_t *) TIMESCALEADDR)))

/* Creates a variable that contains the process state stored in the biosdatapage before an interrupt */
#define GET_BDP_STATUS(T) state_t* T = (state_t*) BIOSDATAPAGE

#endif 