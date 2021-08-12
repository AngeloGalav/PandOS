#include "../include/exception_handler.h"
#include "../include/debugger.h"

extern pcb_PTR currentProcess;
int excCode;

void exceptionHandler()
{
    // The BIOS has now saved the processor state in the BIOSDATAPAGE
    
    GET_BDP_STATUS(exceptionState); /* Retrieve the info stored in the BIOSDATAPAGE */
    unsigned int exceptionCode =  exceptionState->cause & 0x3c; /* Extraction of the ExecCode (exception code) */
    exceptionCode >>= 2;
    excCode = exceptionCode;
    
    if (exceptionCode == 0){ /* Device Interrupt handler */
        InterruptHandler(exceptionState->cause);
        }

    else if ((exceptionCode >= 1) && (exceptionCode <= 3)){ /* TLB exception handler */
        PassUpOrDie(PGFAULTEXCEPT, exceptionState);
        }
         
    else if (exceptionCode == 8) /* Syscall exception handler */
    {
        if (!(BitExtractor(exceptionState->status, 3, 1)))     /* checking if KU bit is off (kernel mode on) */
            SyscallExceptionHandler(exceptionState);
        else
        {
            exceptionState->cause = RESVINSTR;   // if the syscall was executed in user mode, we simulate
            exceptionHandler();                  // a program trap exception
        }
    }
    else{ /* Program Trap exception */
        PassUpOrDie(GENERALEXCEPT, exceptionState);
    }
}

void PassUpOrDie(int except_type, state_t* exceptionState)
{
    if (currentProcess->p_supportStruct == NULL) { 
        bp_correct(); Terminate_Process_SYS2(); }// Die (process termination)
    else // PassUp
    {
        (currentProcess->p_supportStruct)->sup_exceptState[except_type] = *exceptionState; 
        context_t info_to_pass = (currentProcess->p_supportStruct)->sup_exceptContext[except_type]; // passing up the support info
        LDCXT(info_to_pass.stackPtr, info_to_pass.status, info_to_pass.pc);                   // to the support level
    }
}
