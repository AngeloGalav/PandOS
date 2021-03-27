#include "../Libraries/exception_handler.h"

extern pcb_PTR currentProcess;

void fooBar()
{
    state_t *exceptionState = (memaddr) BIOSDATAPAGE;
    currentProcess->p_s = *exceptionState; // updates the current process status
    unsigned int exceptionCode = (unsigned int) exceptionState->cause & 124; // extract ExecCode from cause register
    exceptionCode >>= 2;
    
    if(exceptionCode == 0)
    {
        // Kernel device interrupt handler (3.6 pandos)
    }
    else if ((exceptionCode >= 1) && (exceptionCode <= 3))
    {
        // Kernel TLB exception handler (3.7.3 pandos)
    }
    else if (exceptionCode == 8)
    {
        // SYSCALL exception handler (3.5 pandos)
        /**
        * In particular, if the process making a SYSCALLrequest was in kernel-mode and a0 contained a
        * value in the range [1..8] then the Nucleus should perform one of the services
        * Leggere il valore di a0 che contiene l'indicazione alla SYSCALL da invocare dopo aver controllato
        * se la kernel mode fosse attiva o meno.
        **/    
       if (checkMode(exceptionState->status))
            SyscallExceptionHandler(exceptionState); 
       else
       {
           //Call some sort of trap
       }
    }
    else
    {
         //* Code 4-7,9-12: Kernel Program Trap exception handler (3.7.2 pandos)  
    }
}

HIDDEN void SyscallAccessDenied()
{
   
}