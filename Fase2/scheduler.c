#include "../Libraries/scheduler.h"
#include "../Libraries/debugger.h"

extern pcb_PTR readyQueue;
extern int softBlockCount;
extern int processCount;
extern pcb_PTR currentProcess;

cpu_t startTime;

void Scheduler()
{   
    if (currentProcess != NULL)
        currentProcess->p_time += (CURRENT_TOD - startTime);

    currentProcess = removeProcQ(&readyQueue);   
    
    if(currentProcess != NULL) /* If the queue is not null */
    {  
        STCK(startTime);
        setTIMER(TIMERVALUE(PSECOND)); /* setting the timeslice */
        LDST(&(currentProcess->p_s));   /* loading the process state */
    }
    else // readyQueue is now empty
    {
        if (processCount == 0)
            HALT();
        if(processCount > 0 && softBlockCount > 0)
        {
            setTIMER(TIMERVALUE(MAXINT));
            setSTATUS(IECON | IMON);
            WAIT();
        }
        else if(processCount > 0 && softBlockCount == 0) // deadlock induced kernel panic
            PANIC();
    }
}