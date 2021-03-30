#include "../Libraries/syscall.h"

extern pcb_PTR readyQueue;
HIDDEN state_t* cached_exceptionState; ///TODO: Re-integrate for caching
extern pcb_PTR currentProcess;

extern unsigned int processCount;
extern unsigned int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];

void SyscallExceptionHandler(state_t* exception_state)
{
    cached_exceptionState = exception_state;
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

    initializePcb(newproc);
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
    outChild(currentProcess);
    KillRec(currentProcess->p_child);
    TerminateSingleProcess(currentProcess);
}

HIDDEN void TerminateSingleProcess(pcb_t* to_terminate) // static because it can only be called in this file scope.
{
    // Se il valore del semaforo e' negativo, allora il numero dei processi bloccati corrisponde in quel semaforo corrisponde
    // al valore assoluto del valore del semaforo stesso. Questo valore, se un processo bloccato viene terminato, va aggiustato.
    processCount -= 1;
    outProcQ(&readyQueue, to_terminate);
    
    
    if (to_terminate->p_semAdd != NULL)
    {   
        // Device semaphore check && elimination from semaphore
        if (!(to_terminate->p_semAdd >= &device_semaphores[0] && to_terminate->p_semAdd <= &device_semaphores[48]) 
            && removeBlocked(to_terminate->p_semAdd) != NULL)
        {
            if (*(to_terminate->p_semAdd) < 0) *(to_terminate->p_semAdd) += 1;
            softBlockCount -= 1;
        }
    }

    freePcb(to_terminate);
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

    if (*semAddr < 0) // if the value is negative, the process gets blocked
    {
        currentProcess->p_semAdd = semAddr;
        softBlockCount += 1;
        
        insertBlocked(semAddr, currentProcess); /* currentProcess is now in blocked state */
        outProcQ(&readyQueue, currentProcess);

        currentProcess->p_time += TIMERVALUE((*((memaddr *) TODLOADDR))) - currentProcess->untracked_TOD_mark; 
        currentProcess->p_s = *cached_exceptionState;
        currentProcess->p_s.pc_epc += 4;        
        
        Scheduler();
    }
}

pcb_t* Verhogen_SYS4(int* semAddr) 
{
    *semAddr += 1;
    pcb_t* unlockedProcess = removeBlocked(semAddr);

    if (unlockedProcess != NULL) 
    {
        unlockedProcess->p_semAdd = NULL;
        softBlockCount -= 1;
        insertProcQ(&readyQueue, unlockedProcess);
    }

    cached_exceptionState->pc_epc += 4;

    return unlockedProcess;
}

void Wait_For_IO_Device_SYS5(int intlNo, int dnum, int waitForTermRead)
{
    if (intlNo == 7) dnum = 2 * dnum + waitForTermRead;
    int index = (intlNo - 3) * 8 + dnum;

    cached_exceptionState->reg_v0 = GetStatusWord(intlNo, dnum, waitForTermRead);
    Passeren_SYS3(&device_semaphores[index]);
}

void Get_CPU_Time_SYS6()
{
    cached_exceptionState->reg_v0 = currentProcess->p_time + 
        (TIMERVALUE((*((memaddr *) TODLOADDR))) - currentProcess->untracked_TOD_mark);
}

void Wait_For_Clock_SYS7() 
{
    Passeren_SYS3(&device_semaphores[48]);
}

void Get_Support_Data_SYS8()
{
    cached_exceptionState->reg_v0 = currentProcess->p_supportStruct;
}

void SYSCALL_Return()
{
    cached_exceptionState->pc_epc += 4;
    LDST(cached_exceptionState);
}
