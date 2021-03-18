#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#define INIT_STATUS 0x8000004

#define SEMAPHORE_QTY 49

/* Define the 100000 milliseconds */
#define TIMERVALUE(T)  ((T) * (*((cpu_t *) TIMESCALEADDR)))

#define DEVICES_NUMBER 17



#endif 