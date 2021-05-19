#include "../include/sysSupport.h"
#include "libraries.h"


void GeneralException_Handler ()
{
    //We take the support info of the current process with the SYS8
    support_t *sPtr = SYSCALL(GETSPTPTR, 0, 0, 0);
    //extract the execCode to check if there is a SYSCALL or a TRAP exception
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);
}
