#include "../Libraries/devinterrupt_handler.h"

extern pcb_PTR readyQueue;

extern pcb_PTR currentProcess;

extern int device_semaphores[SEMAPHORE_QTY];

void InterruptPendingChecker(unsigned int cause_reg)
{
    
    unsigned int ip_reg = BitExtractor(cause_reg, 0xFF00, 8);
    int mask = 2;
    if(ip_reg & 1)
        PANIC();
    for (int i = 1; i < 8; i++)
    {
        if (ip_reg & mask) 
            InterruptLineDeviceCheck(i);
        mask *= 2;
    }
    
        
}

void InterruptLineDeviceCheck(int line)
{
    unsigned int * device = (memaddr) IDEVBITMAP + (line * 0x4);
    int devices = 8;
    if(line == 7)
    {
        devices = 16;
    }
    if((line>=3) && (line<=6))
    {
        int mask = 1;
        for (int i = 0; i<devices; i++)
        {
            if(*device & mask)
                InterruptHandler(line, i);
            mask *=2;
        }
    }
}

void InterruptHandler(int line, int device)
{
    devreg_t * device_register = (devreg_t*) (0x10000054 + ((line - 3) * 0x80) + (device * 0x10));
    unsigned int status_word = GetStatusWord(line, device, device % 2);
    if((line >= 3) && (line <= 6))
    {
        device_register->dtp.command = ACK;
    }
    else if (line == 7)
    {
        if(device_register->term.recv_status)
        {
            device_register->term.recv_command = ACK;
        }
        if(device_register->term.transm_status)
        {
            device_register->term.transm_command = ACK;
        }
    }
    else if (line == 1)
    {

    }

    if (line == 7) device = 2 * device + device % 2; //+ waitForTermRead; Risolvere il fatto dei 8/16 device per il terminale
    int index = (line - 3) * 8 + device; 
    Verhogen_SYS4(&device_semaphores[index]);
    currentProcess->p_s.reg_v0 = status_word;
    insertProcQ(&readyQueue, currentProcess);
    GET_STATUS(status_to_load);
    LDST(status_to_load);
}