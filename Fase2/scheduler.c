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

void scheduler()
{
    if (!emptyProcQ(readyQueue))
    {
        currentProcess = removeProcQ(&readyQueue);
        int status = setTIMER(TIMERVALUE(5000));
        LDST ((state_t *) &(currentProcess->p_s)); // 

        //IF BLOCKING SYSCALL PUT THE PROCESS IN THE EVENT QUEUE
        //ELSE IF NON-BLOCKING SYSCALL PUT THE PROCESS IN THE READY QUEUE WITH ROUNDROBIN
        
    } else // IF  READY QUEUE IT'S EMPTY
    {
        if (processCount == 0)
        {
            //TO-DO we should be in KERNEL MODE
            HALT();
        }
        else if ((processCount && softBlockCount) > 0)
        {
            setSTATUS(WAIT_STATUS);
            WAIT();
        }
        else if ((softBlockCount == 0) && (processCount > 0))
        {
            PANIC();
        }
    }
}