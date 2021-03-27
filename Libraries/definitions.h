#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#define INIT_STATUS 0x8000004 

#define WAIT_STATUS 0x0000001 // Disables PLT interrupts and activates 

/* Total number of semaphores in PandOS */
#define SEMAPHORE_QTY 49

/* Timer converter to get the right time */
#define TIMERVALUE(T)  ((T) * (*((cpu_t *) TIMESCALEADDR)))

/* Value to assign to ExeCode of cause register if entering a syscall in usermode */
#define RESERVEDINSTRUCTION 10

#endif 