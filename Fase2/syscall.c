#include "../include/syscall.h"

extern pcb_PTR readyQueue;
HIDDEN state_t* cached_exceptionState; /* we cache the exception state on the BIOSDATAPAGE */
extern pcb_PTR currentProcess;

extern cpu_t startTime;

extern int processCount;
extern int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];

void SyscallExceptionHandler(state_t* exception_state)
{
    cached_exceptionState = exception_state;
    unsigned int sysCallCode = (unsigned int) exception_state->reg_a0;
    cached_exceptionState->pc_epc += WORDLEN; /* we increase the PC of a WORD to avoid looping syscall callings */
    
    switch (sysCallCode)
    {
        case CREATEPROCESS:
            Create_Process_SYS1(); 
            break;
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
            Wait_For_IO_Device_SYS5();
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
            InvalidSyscall();
            break;
    } 
}

void Create_Process_SYS1()
{
    state_t statep = *((state_t*) cached_exceptionState->reg_a1); /* Gathering info from the state in the BIOSDATAPAGE */
    support_t *supportp = (support_t*) cached_exceptionState->reg_a2;

    pcb_PTR newproc = allocPcb(); /* pcb allocation */

    if (newproc != NULL) /* If we can allocate all the resources needed... */
    {
        initializePcb(newproc);
        insertProcQ(&(readyQueue), newproc);
        insertChild(currentProcess, newproc);
        currentProcess->p_s.reg_v0 = OK;    /* process creation successful, sending 0... */

        processCount += 1;

        newproc->p_semAdd = NULL;
        newproc->p_time = 0; 
        newproc->p_s = statep;

        newproc->p_supportStruct = supportp; /* adding support level "support" (if provided) */
    } 
    else currentProcess->p_s.reg_v0 = NOPROC; /* process creation unsuccesful, sending -1... */

    LDST(cached_exceptionState);
}


void Terminate_Process_SYS2()
{
    outChild(currentProcess);   /* */
    TerminateTree(currentProcess); /*  */    
    
    currentProcess = NULL;
    Scheduler();
}

HIDDEN void TerminateTree(pcb_t* to_terminate)
{
    if (to_terminate == NULL) return;

    while (!(emptyChild(to_terminate)))
        TerminateTree(removeChild(to_terminate));   /*  */

    TerminateSingleProcess(to_terminate);   /*  */
}

HIDDEN void TerminateSingleProcess(pcb_t* to_terminate) // declared static so it can only be called in this file's scope.
{
    processCount -= 1;
    outProcQ(&readyQueue, to_terminate);    /* we remove the terminated process from the readyQueue */
    
    if (to_terminate->p_semAdd != NULL)
    {   
        /* Device semaphore check && elimination from semaphore */
        if (&(device_semaphores[0]) <= to_terminate->p_semAdd && to_terminate->p_semAdd <= &(device_semaphores[48]))
            softBlockCount -= 1; // the process is blocked in a device sem
        else
            *(to_terminate->p_semAdd) += 1; // the process is blocked in a normal sem
        
        outBlocked(to_terminate);
    }
    freePcb(to_terminate);  /* we free the resources utilized by the terminated process */
}

void Passeren_SYS3(int* semAddr) 
{   
    *semAddr -= 1;

    if (*semAddr < 0) // if the value is negative, the process gets blocked
    {   
        currentProcess->p_s = *cached_exceptionState; 
        insertBlocked(semAddr, currentProcess); /* currentProcess is now in blocked state */
        Scheduler();
    }
    else LDST(cached_exceptionState);
}

void Verhogen_SYS4(int* semAddr) 
{
    *semAddr += 1;
    pcb_t* unlockedProcess = removeBlocked(semAddr);

    if (unlockedProcess != NULL) 
    {
        unlockedProcess->p_semAdd = NULL;
        insertProcQ(&readyQueue, unlockedProcess);
    }

    LDST(cached_exceptionState);
}

void Wait_For_IO_Device_SYS5()
{
    int intlNo = cached_exceptionState->reg_a1;
    int dnum = cached_exceptionState->reg_a2;
    int waitForTermRead = cached_exceptionState->reg_a3;

    if (intlNo == 7) dnum = 2 * dnum + waitForTermRead;
    int index = (intlNo - 3) * 8 + dnum;

    /* performing a V on the device semaphore */
    softBlockCount += 1;
    Passeren_SYS3(&(device_semaphores[index]));
}

void Get_CPU_Time_SYS6()
{
    currentProcess->p_time += (CURRENT_TOD - startTime); 
    cached_exceptionState->reg_v0 = currentProcess->p_time; 
    STCK(startTime);
    LDST(cached_exceptionState);
}

void Wait_For_Clock_SYS7() 
{
    softBlockCount += 1;
    Passeren_SYS3(&device_semaphores[SEMAPHORE_QTY - 1]); /* we perform a P on the interval timer semaphore */
}

void Get_Support_Data_SYS8()
{
    cached_exceptionState->reg_v0 = (unsigned int) currentProcess->p_supportStruct;
    LDST(cached_exceptionState);
}

void InvalidSyscall()
{
    if (currentProcess->p_supportStruct == NULL) Terminate_Process_SYS2(); /* Die phase */
    else
    {
        (currentProcess->p_supportStruct)->sup_exceptState[GENERALEXCEPT] = *cached_exceptionState;
        context_t info_to_pass = (currentProcess->p_supportStruct)->sup_exceptContext[GENERALEXCEPT];
        LDCXT(info_to_pass.c_stackPtr, info_to_pass.c_status, info_to_pass.c_pc);
    }
}
