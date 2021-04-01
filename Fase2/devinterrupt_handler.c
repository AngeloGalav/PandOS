#include "../Libraries/devinterrupt_handler.h"
#include "../Libraries/debugger.h"

extern pcb_PTR readyQueue;

state_t* status_to_ll;

extern pcb_PTR currentProcess;

unsigned int * device; ///TODO: TOGLI E DICHIARA DENTRO LA FUNZIONE

extern unsigned int softBlockCount;

extern int device_semaphores[SEMAPHORE_QTY];

void InterruptPendingChecker(unsigned int cause_reg)
{
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);
    int mask = 2;
    // if(ip_reg & 1)
    //     PANIC();
    
    for (int i = 1; i < 8; i++)
    {
        if (ip_reg & mask) 
            InterruptLineDeviceCheck(i);
        mask *= 2;
    }

    ///TODO: non-device interrupts    
}

void InterruptLineDeviceCheck(int line)
{
    if (line > 2) /* Non-timer device interrupt line*/
    {
        device = (memaddr*) (IDEVBITMAP + ((line - 3) * 0x4));

        int mask = 1;
        for (int i = 0; i < DEVPERINT; i++)
        {
            if (*device & mask)
                InterruptHandler(line, i);
            mask *= 2;
        }
    }
    else if (line == 1) /* PLT timer interrupt line */
    {

        setTIMER(TIMERVALUE(5000));
        currentProcess->p_s = *((state_t*) BIOSDATAPAGE);
        insertProcQ(&readyQueue, currentProcess);
        
        Scheduler();
    }
    else /* Interval timer interrupt line */
    {
        bp_interval();
        LDIT(PSECOND);

        if (headBlocked(&device_semaphores[SEMAPHORE_QTY - 1]) == NULL)
        {
            //bp();
        }
        
        while(headBlocked(&device_semaphores[SEMAPHORE_QTY - 1]) != NULL)
        {
            insertProcQ(readyQueue, removeBlocked(&device_semaphores[SEMAPHORE_QTY - 1]));
            softBlockCount -= 1;
        }
        
        device_semaphores[SEMAPHORE_QTY - 1] = 0;
    
        //bp_extra();

        LDST((state_t*) BIOSDATAPAGE);
    }
}

void InterruptHandler(int line, int device)
{
    devreg_t * device_register = (devreg_t*) (0x10000054 + ((line - 3) * 0x80) + (device * 0x10));
    unsigned int status_word = GetStatusWord(line, device, device % 2);
    
    if((line >= 3) && (line <= 6))
        device_register->dtp.command = ACK;
    else if (line == 7)
    {
        device *= 2;

        if(device_register->term.recv_status != READY)
        {
            device_register->term.recv_command = ACK;
            device += 1;
        }

        if(device_register->term.transm_status != READY)
            device_register->term.transm_command = ACK; // <- arriva qui
    }

    int index = (line - 3) * 8 + device; 
    
    pcb_t* resumedProcess = Verhogen_SYS4(&device_semaphores[index]);

    if (resumedProcess != NULL) 
    {
        bp_correct();   
        resumedProcess->p_s.reg_v0 = status_word;
    }

    bp_device();
    LDST((state_t *) BIOSDATAPAGE);
}