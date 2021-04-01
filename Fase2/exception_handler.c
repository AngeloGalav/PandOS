#include "../Libraries/exception_handler.h"
#include "../Libraries/debugger.h"

extern pcb_PTR currentProcess;
int TOD_timer_start;

//Change this name
void fooBar()
{
    // The BIOS has now saved the processor state in the BIOSDATAPAGE

    STCK(TOD_timer_start);
    
    GET_BDP_STATUS(exceptionState); /* Retrieve the info stored in the BIOSDATAPAGE */
    currentProcess->p_s = *exceptionState; /* We update the status of the current process*/
    
    unsigned int exceptionCode = (unsigned int) exceptionState->cause & 124; /* Extraction of the ExecCode (exception code)*/
    exceptionCode >>= 2;
    
    if (exceptionCode == 0) /* Device Interrupt handler */
    {  
        InterruptPendingChecker(exceptionState->cause);
    }
    else if ((exceptionCode >= 1) && (exceptionCode <= 3)) /* TLB exception handler */
    {
        if (currentProcess->p_supportStruct == NULL) Terminate_Process_SYS2();
        else PassUp(PGFAULTEXCEPT, exceptionState);
    }
    else if (exceptionCode == 8) /* Syscall exception handler */
    {
        if (!(checkMode(exceptionState->status)))
        {                                               // TODO: in questo caso mi sa che dobbiamo controllare il KUc bit
            SyscallExceptionHandler(exceptionState); 
            SyscallReturn();
        }
        else
        {
            setExcCode(exceptionState, RESERVEDINSTRUCTION);
        }
    }
    else /* Program Trap exception */
    {
        if (currentProcess->p_supportStruct == NULL) Terminate_Process_SYS2();
        else PassUp(GENERALEXCEPT, exceptionState);
    }
}

void PassUp(int except_type, state_t* exceptionState)
{
    (currentProcess->p_supportStruct)->sup_exceptState[except_type] = *exceptionState;
    context_t info_to_pass = (currentProcess->p_supportStruct)->sup_exceptContext[except_type];
            
    LDCXT(info_to_pass.c_stackPtr, info_to_pass.c_status, info_to_pass.c_pc);
}
