#include "../Libraries/scheduler.h"

/* Tail pointer to a queue of pcbs that are in the “ready” state. */
pcb_PTR readyQueue;

/** Number of started, but not terminated processes that in are the
*   “blocked” state  due to an I/O or timer request 
*/
unsigned int softBlockCount = 0;

/* Number of started, but not yet terminated processes. */
unsigned int processCount;

/* Pointer to the current pcb that is in running state */ 
pcb_PTR currentProcess = NULL;

void Scheduler()
{
    if (!emptyProcQ(readyQueue))
    {
        currentProcess = removeProcQ(&readyQueue);
        int status = setTIMER(TIMERVALUE(5000));    /// TODO: chiedere al maldus se questa riga va bene
        LDST ((state_t *) &(currentProcess->p_s)); 
        
    } else // If ready queue is empty
    {
        if (processCount == 0)
            HALT(); // If the executing process is not in kernel mode
        else if ((processCount && softBlockCount) > 0)
        {
            setSTATUS(WAIT_STATUS);
            WAIT();
        }
        else if ((softBlockCount == 0) && (processCount > 0))
            PANIC(); // Deadlock caused kernel panic
    }
}