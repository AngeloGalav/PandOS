#include "../include/scheduler.h"

extern pcb_PTR readyQueue;
extern int softBlockCount;
extern int processCount;
extern pcb_PTR currentProcess;

cpu_t startTime;

void Scheduler()
{   
    if (currentProcess != NULL)
        currentProcess->p_time += (CURRENT_TOD - startTime);  /* we update the CPU time since the this process is probably blocked */

    currentProcess = removeProcQ(&readyQueue);  /* Dispatching a process */
    
    if(currentProcess != NULL) /* If the queue is not null */
    {  
        STCK(startTime);
        setTIMER(TIMERVALUE(PSECOND)); /* setting the timeslice */
        LDST(&(currentProcess->p_s));   /* loading the process state and giving control to the newly dispatched process */
    }
    else // readyQueue is now empty
    {
        if (processCount == 0)
            HALT();
            
        if(processCount > 0 && softBlockCount > 0) /* we haven't got anymore processes to dispatch... */
        {
            setTIMER(TIMERVALUE(MAXINT));
            setSTATUS(IECON | IMON);
            WAIT();                     /* ...so we are waiting for an interrupt to occur.  */
        }
        else if(processCount > 0 && softBlockCount == 0) // deadlock induced kernel panic
            PANIC();
    }
}