#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#define INIT_STATUS 0x8000004

#define WAIT_STATUS 0x0000001

#define SEMAPHORE_QTY 49

/* Timer converter to get the right time */
#define TIMERVALUE(T)  ((T) * (*((cpu_t *) TIMESCALEADDR)))

#define DEVICES_NUMBER 17



#endif 