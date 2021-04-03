#include "../Libraries/devinterrupt_handler.h"
#include "../Libraries/debugger.h"

extern pcb_PTR readyQueue;
extern pcb_PTR currentProcess;

extern int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];
extern cpu_t startTime;

cpu_t interruptStartTime;

void getInterruptLine(unsigned int cause_reg)
{
    STCK(interruptStartTime);
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);
    int mask = 2;
    
    if(ip_reg & 1)
         PANIC();
    
    for (int i = 1; i < 8; i++)
    {
        if (ip_reg & mask) 
            GeneralIntHandler(i);
        mask *= 2;
    }
}

void GeneralIntHandler(int line)
{
    if (line > 2) /* Non-timer device interrupt line*/
    {
        memaddr* device = (memaddr*) (IDEVBITMAP + ((line - 3) * 0x4));
        int mask = 1;
        for (int i = 0; i < DEVPERINT; i++)
        {
            if (*device & mask)
                NonTimerHandler(line, i);
            mask *= 2;
        }
    }
    else if (line == 1) /* PLT timer interrupt line */
    {
        setTIMER(TIMERVALUE(__INT32_MAX__));
        currentProcess->p_s = *((state_t*) BIOSDATAPAGE);
        currentProcess->p_time += (CURRENT_TOD - startTime); 
        insertProcQ(&readyQueue, currentProcess);
        Scheduler();
    }
    else /* Interval timer interrupt line */
    {
        LDIT(PSECOND);

        while (headBlocked(&device_semaphores[SEMAPHORE_QTY - 1]) != NULL)
        {
            pcb_t* unlockedProcess = removeBlocked(&device_semaphores[SEMAPHORE_QTY - 1]);
            
            if (unlockedProcess != NULL)
            {
                unlockedProcess->p_semAdd = NULL;
                unlockedProcess->p_time += CURRENT_TOD - interruptStartTime;
                insertProcQ(&readyQueue, unlockedProcess);
                softBlockCount -= 1;
            }
        }
        
        device_semaphores[SEMAPHORE_QTY - 1] = 0;

        if (currentProcess == NULL) Scheduler();
        else LDST((state_t*) BIOSDATAPAGE);
    }
}

void NonTimerHandler(int line, int device)
{
    devreg_t * device_register = (devreg_t*) (0x10000054 + ((line - 3) * 0x80) + (device * 0x10));
    int isReadTerm = 0;
    unsigned int status_word;
    
    if (2 < line && line < 7)
    {
        device_register->dtp.command = ACK;
        status_word = device_register->dtp.status;
    }
    else if (line == 7)
    {
        termreg_t* term_register = (devreg_t*) device_register;
       
        if(term_register->recv_status != READY)
        {
            status_word = term_register->recv_status;
            term_register->recv_command = ACK;
            isReadTerm = 1;
        }
        else 
        {
            status_word = term_register->transm_status;
            term_register->transm_command = ACK;
        }

        device = 2*device + isReadTerm;
    }
    
    int index = (line - 3) * 8 + device; 

    device_semaphores[index] += 1;
    pcb_t* unlockedProcess = removeBlocked(&device_semaphores[index]);

    if (unlockedProcess != NULL) 
    {
        unlockedProcess->p_s.reg_v0 = status_word;
        unlockedProcess->p_semAdd = NULL; 
        unlockedProcess->p_time += CURRENT_TOD - interruptStartTime;
        softBlockCount -= 1;
        insertProcQ(&readyQueue, unlockedProcess);
    }

    if (currentProcess == NULL) Scheduler();
    else LDST((state_t *) BIOSDATAPAGE);
}