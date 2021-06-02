#include "../include/devinterrupt_handler.h"

extern pcb_PTR readyQueue;
extern pcb_PTR currentProcess;

extern int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];
extern cpu_t startTime;

/* we keep track of the start time of the interrupt */
cpu_t interruptStartTime;

void InterruptHandler(unsigned int cause_reg)
{
    STCK(interruptStartTime);
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);  // we extract the IP part of the cause register
    int mask = 2;   // mask used to cycle ip_reg bit by bit (set to 2 because we start at interrupt line 1)
    
    if(ip_reg & 1)  // in case it's a line 0 interrupt, we cant handle it (for now)
         PANIC();
    
    for (int i = 1; i < 8; i++) // we cycle through the bits to see which one is requesting to be handled
    {
        if (ip_reg & mask) 
            InterruptLineHandler(i);
        mask *= 2;
    }
}

void InterruptLineHandler(int line)
{
    if (line > 2) /* Non-timer device interrupt line*/
    {
        memaddr* device = (memaddr*) (IDEVBITMAP + ((line - 3) * 0x4));
        int mask = 1;
        for (int i = 0; i < DEVPERINT; i++)     // in case its a line >2 interrupt, we cycle through its devices
        {                                       // to look for the one that we have to handle
            if (*device & mask)
                NonTimerHandler(line, i);
            mask *= 2;
        }
    }
    else if (line == 1) /* PLT timer interrupt line */
    {
        setTIMER(TIMERVALUE(__INT32_MAX__));    // setting the timer to a high value
        currentProcess->p_s = *((state_t*) BIOSDATAPAGE);   // we update the current process state information
        currentProcess->p_time += (CURRENT_TOD - startTime); 
        insertProcQ(&readyQueue, currentProcess);   // we re-insert the process in the readyqueue
        Scheduler();
    }
    else /* Interval timer interrupt line, LINE 2 */
    {
        LDIT(PSECOND);

        /* unlocking all processes in the interval timer semaphore */
        while (headBlocked(&device_semaphores[INTERVALTIMER_INDEX]) != NULL)
        {
            pcb_t* unlockedProcess = removeBlocked(&device_semaphores[INTERVALTIMER_INDEX]);
            
            if (unlockedProcess != NULL)    // equivalent of performing a continuos V operation the 
            {                               // interval timer semaphore
                unlockedProcess->p_semAdd = NULL;
                
                /* we charge the unlocked process with the time spent handling the interrupt */
                unlockedProcess->p_time += CURRENT_TOD - interruptStartTime;
                insertProcQ(&readyQueue, unlockedProcess);
                softBlockCount -= 1;
            }
        }
        
        device_semaphores[INTERVALTIMER_INDEX] = 0; // we adjust the semaphore value

        if (currentProcess == NULL) Scheduler(); /* passing control to the current process (if there is one) */
        else LDST((state_t*) BIOSDATAPAGE);
    }
}

void NonTimerHandler(int line, int device)
{
    devreg_t* device_register = (devreg_t*) (DEVREG + ((line - 3) * 0x80) + (device * 0x10)); //DA QUI CAPIAMO IL NUMERO DEL DEVICE
    int isReadTerm = 0;
    unsigned int status_word;
    
    if (2 < line && line < 7)  // if it's a normal device
    {
        device_register->dtp.command = ACK; // sending the ack
        status_word = device_register->dtp.status;
    }
    else if (line == 7) // in this case, it's a terminal sub-device
    {
        termreg_t* term_register = (termreg_t*) device_register;
       
        if (term_register->recv_status != READY)    // if the terminal is receiver (aka read)
        {
            status_word = term_register->recv_status;
            term_register->recv_command = ACK;
            isReadTerm = 1;
        }
        else   // if the terminal is transmitter (aka write)
        {
            status_word = term_register->transm_status;
            term_register->transm_command = ACK;
        }

        device = 2 * device + isReadTerm;   // we modify the calculation formulae used for finding
    }                                       // the semaphore index if line == 7
    
    int index = (line - 3) * 8 + device; 

    /* equivalent of a V operation on the device semaphore */
    device_semaphores[index] += 1;
    pcb_t* unlockedProcess = removeBlocked(&device_semaphores[index]);

    if (unlockedProcess != NULL) 
    {
        unlockedProcess->p_s.reg_v0 = status_word;
        unlockedProcess->p_semAdd = NULL; 

        /* we charge the unlocked process with the time spent handling the interrupt */
        unlockedProcess->p_time += CURRENT_TOD - interruptStartTime;
        insertProcQ(&readyQueue, unlockedProcess); // the unlocked process is now in ready state
        softBlockCount -= 1;
    }

    if (currentProcess == NULL) Scheduler();  /* passing control to the current process (if there is one) */
    else LDST((state_t *) BIOSDATAPAGE);
}