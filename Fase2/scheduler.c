#include "../Libraries/scheduler.h"
#include "../Libraries/debugger.h"

extern pcb_PTR readyQueue;
extern int softBlockCount;
extern int processCount;
extern pcb_PTR currentProcess;

cpu_t startTime;

void Scheduler()
{    
    currentProcess = removeProcQ(&readyQueue);   
    if(currentProcess != NULL)
    {
        setTIMER(TIMERVALUE(TIMESLICE)); // setting the timeslice
        STCK(startTime);
        LDST(&(currentProcess->p_s));   // loading the process state
    }
    else // readyQueue is now empty
    {
        if (processCount == 0)
            HALT();
        if(processCount > 0 && softBlockCount > 0)
        {
            setTIMER(TIMERVALUE(__INT32_MAX__));
            bp_wait();
            setSTATUS(IECON | IMON);
            bp_wait2();
            WAIT();
        }
        else if(processCount > 0 && softBlockCount == 0) // deadlock induced kernel panic
            PANIC();
    }
}