#include "../Libraries/exception_handler.h"

extern pcb_PTR currentProcess;

//Change this name
void fooBar()
{
    STCK(currentProcess->untracked_TOD_mark);
    
    //GET_BDP_STATUS(exceptionState);
    state_t *exceptionState = (state_t*) BIOSDATAPAGE;
    currentProcess->p_s = *exceptionState; // updates the current process status
    unsigned int exceptionCode = (unsigned int) exceptionState->cause & 124; // extract ExecCode from cause register
    exceptionCode >>= 2;
    
    if (exceptionCode == 0)
    {   
        // Kernel device interrupt handler (3.6 pandos)
        InterruptPendingChecker(exceptionState->cause);
    }
    else if ((exceptionCode >= 1) && (exceptionCode <= 3))
    {
        // Kernel TLB exception handler (3.7.3 pandos)
    }
    else if (exceptionCode == 8) /* Syscall exception handler */
    {
        if (!(checkMode(exceptionState->status)))
        {                                               // TODO: in questo caso mi sa che dobbiamo controllare il KUc bit
            SyscallExceptionHandler(exceptionState); 
            SYSCALL_Return();
        }
        else
        {
           setExcCode(exceptionState, RESERVEDINSTRUCTION);
           //fooBar();
        }
    }
    else
    {
         //* Code 4-7,9-12: Kernel Program Trap exception handler (3.7.2 pandos)  
    }
}