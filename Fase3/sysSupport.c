#include "../include/sysSupport.h"
#include "syscall.h"
#include "libraries.h"


void GeneralException_Handler ()
{
    //We take the support info of the current process with the SYS8
    support_t *sPtr = SYSCALL(GETSPTPTR, 0, 0, 0);
    //extract the execCode to check if there is a SYSCALL or a TRAP exception
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);
    
    if(excCode >= 9)
    {
        Support_Syscall_Handler(sPtr); 
    }
    else // è tra 4 e 7 compresi
    {
        //PROGRAM TRAP HANDLER
    }
}

void Support_Syscall_Handler(support_t *sPtr)
{
    unsigned int sysnumber = (unsigned int) sPtr->sup_exceptState->reg_a0;

    switch (sysnumber)
    {
    case TERMINATE:

        Terminate_SYS9();

        break;

    case GET_TOD:

        Get_Tod_SYS10((unsigned int*) sPtr->sup_exceptState->reg_v0);
        break;

    case WRITEPRINTER:
       
       Write_To_Printer_SYS11(sPtr);
        break;
    
    case WRITETERMINAL:
       
        break;

    case READTERMINAL:
       
        break;

    default:
        //PANIC ?
        break;
    }
    //increment the program counter by 4 here or somewhere else ??
}

void Terminate_SYS9() // sys2 wrapper ?
{
    Terminate_Process_SYS2();
}


void Get_Tod_SYS10(unsigned int *regv0) // we need to check if the pointers are right
{
    unsigned int tod;
    STCK(tod);// macro that takes unsigned int and populate it with TOD value
    *regv0 = (unsigned int) tod;
}

void  Write_To_Printer_SYS11(support_t* sPtr)
{
    // Printer device associated with the U-proc 
    // IntLno 6
    // DevNo = current Asid 
    devreg_t* devReg = DEV_REG_ADDR(6, sPtr->sup_asid);
    // Is an error to write to a printer device from an address outside of the requesting 
    // U-proc’s logical address space, but how can we check it ????????
    // Lo scopriremo in un'altra puntata di MISTERO
    // devo mettere da a1 dentro DATA0 per la lunghezza di a2
    //Ogni carattere è un byte quindi ogni carattere è un ciclo in cui faccio :
    // metto il carattere da a1 in DATA0, setto lo status e scrivo il comando per scrivere
    //immagino che in a2 ci sia il numero di caratteri quindi di byte per il mio ciclo
    // inoltre controllo che se ho a2 < 0 || a2 > 128 dò errore quindi SYS9
    unsigned int strlength = (unsigned int) sPtr->sup_exceptState->reg_a2 ;
    if((strlength >= 0 ) && (strlength <= MAXSTRLENG))
    {
        char *s = sPtr->sup_exceptState->reg_a1;
        while( *s != EOF)
        {
            devReg->dtp.data0 = *s;

            devReg->dtp.command = PRINTCHR;

            s++;
            //bisogna controllare che sia andato a buon fine ad ogni iterazione e quindi in caso no riprovare ?
        }
    }
    else
        Terminate_SYS9();
}