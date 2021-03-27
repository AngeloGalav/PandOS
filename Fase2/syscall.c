#include "../Libraries/syscall.h"

extern pcb_PTR readyQueue;

extern pcb_PTR currentProcess;

extern unsigned int processCount;
extern unsigned int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];

void SyscallExceptionHandler(state_t* exception_state)
{
    unsigned int sysCallCode = (unsigned int) exception_state->reg_a0;

    switch (sysCallCode)
    {
        {
        case CREATEPROCESS: ;
            state_t new_pstate = *((state_t*) exception_state->reg_a1);
            support_t *new_suppt = (support_t*) exception_state->reg_a2;
            Create_Process_SYS1(new_pstate, new_suppt); 
            break;
        }
        case TERMPROCESS:
            Terminate_Process_SYS2();
            break;
        case PASSEREN:
            Passeren_SYS3((int*) exception_state->reg_a1);
            break;
        case VERHOGEN:
            Verhogen_SYS4((int*) exception_state->reg_a1);
            break;
        case IOWAIT:
            Wait_For_IO_Device_SYS5(*((int*) exception_state->reg_a1),
            *((int*) exception_state->reg_a2) , *((int*) exception_state->reg_a3));
            break;
        case GETTIME:
            Get_CPU_Time_SYS6();
            break;
        case CLOCKWAIT:
            Wait_For_Clock_SYS7();
            break;
        case GETSUPPORTPTR:
            Get_Support_Data_SYS8(exception_state);
            break;
        default:
            break;
    }
}

void Create_Process_SYS1(state_t arg1, support_t* arg2)
{
    pcb_PTR newproc = allocPcb(); 

    initializePcbt(newproc);
    insertProcQ(&(readyQueue), newproc);
    insertChild(currentProcess, newproc);

    processCount += 1;

    newproc->p_semAdd = NULL;
    newproc->p_time = 0; 
    newproc->p_s = arg1;
    newproc->p_supportStruct = arg2;
}


void Terminate_Process_SYS2()
{
    /* Assuming the current process is the one that executes this functions */
    outChild(currentProcess); // 
    TerminateSingleProcess(currentProcess);
    KillRec(currentProcess->p_child);
}


HIDDEN void TerminateSingleProcess(pcb_t* to_terminate) // static because it can only be called in this file scope.
{
    // Se il valore del semaforo e' negativo, allora il numero dei processi bloccati corrisponde in quel semaforo corrisponde
    // al valore assoluto del valore del semaforo stesso. Questo valore, se un processo bloccato viene terminato, va aggiustato.
    processCount -= 1;
    freePcb(to_terminate);
    
    if (to_terminate->p_semAdd != NULL)
    {   
        // Device semaphore check && elimination from semaphore
        if (!(to_terminate->p_semAdd >= &device_semaphores[0] || to_terminate->p_semAdd <= &device_semaphores[48]) 
            && removeBlocked(to_terminate->p_semAdd) != NULL)
        {
            if (*(to_terminate->p_semAdd) < 0) *(to_terminate->p_semAdd) += 1;
            softBlockCount -= 1;
        }
    }
}

HIDDEN void KillRec(pcb_PTR proc_elem)
{
    if (proc_elem == NULL) return;

    KillRec(proc_elem->p_next_sib);
    KillRec(proc_elem->p_child); // We repeat this recursevely for each child of the process
    TerminateSingleProcess(proc_elem);
}

void Passeren_SYS3(int* semAddr) 
{
    *semAddr -= 1;
    if (*semAddr < 0) 
    {
        insertBlocked(semAddr, currentProcess); // if the value is negative, the process gets blockede
        softBlockCount += 1;     
    }
}

void Verhogen_SYS4(int* semAddr) 
{
    if (*semAddr < 0) *semAddr += 1;
    removeBlocked(semAddr);
}

void Wait_For_IO_Device_SYS5(int intlNo, int dnum, int waitForTermRead)
{
    if (intlNo == 7) dnum = 2 * dnum + waitForTermRead;
    int index = (intlNo - 3) * 8 + dnum;
    Passeren_SYS3(&device_semaphores[index]);
    currentProcess->p_s.reg_v0 = (0x10000054 + ((intlNo - 3) * 0x80) + (dnum * 0x10)) & 0x000F; // we're considering only 
                                                                                                // status word bits
}

void Get_CPU_Time_SYS6()
{
    currentProcess->p_s.reg_v0 = currentProcess->p_time;
}

void Wait_For_Clock_SYS7() 
{
    Passeren_SYS3(&device_semaphores[48]);
    //call scheduler
}

void Get_Support_Data_SYS8()
{
    currentProcess->p_s.reg_v0 = currentProcess->p_supportStruct;
}

HIDDEN void BlockProcess()
{
    
} 

