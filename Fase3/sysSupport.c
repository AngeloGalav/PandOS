#include "../include/sysSupport.h"
#include "syscall.h"
#include "../include/libraries.h"

extern int support_printer_semaphore[UPROCMAX];
extern int support_wterminal_semaphore[UPROCMAX];
extern int support_rterminal_semaphore[UPROCMAX];
extern int support_semaphores[SUPP_SEM_N][UPROCMAX];

void GeneralException_Handler()
{
    //We take the support info of the current process with the SYS8
    support_t *sPtr = SYSCALL(GETSPTPTR, 0, 0, 0);
    //extract the execCode to check if there is a SYSCALL or a TRAP exception
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);
    
    if (excCode >= 9)
        Support_Syscall_Handler(sPtr); 
    else // è tra 4 e 7 compresi
       Terminate_SYS9(sPtr);
}

void Support_Syscall_Handler(support_t *sPtr)
{
    unsigned int sysnumber = (unsigned int) sPtr->sup_exceptState[0].reg_a0;

    switch (sysnumber)
    {
        case TERMINATE:
            Terminate_SYS9(sPtr);
            /// TODO: 
            break;

        case GET_TOD:
            Get_Tod_SYS10((unsigned int*) sPtr->sup_exceptState->reg_v0);
            break;

        case WRITEPRINTER:
            Write_To_Printer_SYS11(sPtr);
            break;

        case WRITETERMINAL:
            Write_to_Terminal_SYS12(sPtr);
            break;

        case READTERMINAL:
            Read_From_Terminal_SYS13(sPtr);
            break;

        default:
            //PANIC ?
            break;
    }
        //increment the program counter by 4 here or somewhere else ??
}   

void Terminate_SYS9(support_t* sPtr) // sys2 wrapper 
{  
    // check if the process to be terminated is holding any mutex semaphore
    // in that case use a SYS4 to free it and terminate it with sys2
    
    for (int i = 0; i < SUPP_SEM_N; i++) 
    {
        if (support_semaphores[i][sPtr->sup_asid - 1] <= 0)
            SYSCALL(VERHOGEN, (int) &support_semaphores[i][sPtr->sup_asid - 1], 0, 0); 
    }
    
    Terminate_Process_SYS2();
}


void Get_Tod_SYS10(unsigned int *regv0) 
{
    unsigned int tod;
    STCK(tod);// macro that takes unsigned int and populate it with TOD value
    *regv0 = (unsigned int) tod;
}

void  Write_To_Printer_SYS11(support_t* sPtr)
{
    // Printer device associated with the U-proc 
    devreg_t* devReg = DEV_REG_ADDR(6, sPtr->sup_asid); // prende indirizzo del device register
    unsigned int status;

    // Is an error to write to a printer device from an address outside of the requesting 
    // U-proc’s logical address space, but how can we check it ????????
    
    SYSCALL(PASSERN, (int)&support_printer_semaphore[sPtr->sup_asid - 1], 0, 0); 
    
    unsigned int strlength = (unsigned int) sPtr->sup_exceptState->reg_a2;

    char *s = sPtr->sup_exceptState->reg_a1; 
    // check se l'address è dentro lo spazio del processo, compreso lo stack
    // lo sò l'if così lungo è bruttissimo ma anche la vita è brutta
    if ((strlength >= 0) && (strlength <= MAXSTRLENG) && (*s >= UPROCSTARTADDR) && (*s <= USERSTACKTOP))
    {
    
        while ( *s != EOF)
        {
            devReg->dtp.data0 = *s;

            devReg->dtp.command = PRINTCHR;

            SYSCALL(IOWAIT, 6, sPtr->sup_asid, 0);
            status = devReg->dtp.status;
            
            if (status != DEV0ON)
            {   
                sPtr->sup_exceptState->reg_v0 = status * -1; 
                break;
            }
            s++;
        }

        sPtr->sup_exceptState->reg_v0 = strlength; // VA TUTTO BENEEEE!!! :) ... e i bambini in africa ?
    }
    else
        Terminate_SYS9(sPtr);
    
    SYSCALL(VERHOGEN, (int)&support_printer_semaphore[sPtr->sup_asid - 1], 0 , 0);
}


void Write_to_Terminal_SYS12(support_t* sPtr)
{
   
    devreg_t* devReg = DEV_REG_ADDR(7, sPtr->sup_asid); // prende indirizzo del device register

    unsigned int status;

    SYSCALL(PASSERN, (int)&support_wterminal_semaphore[sPtr->sup_asid - 1], 0, 0); 
    
    unsigned int strlength = (unsigned int) sPtr->sup_exceptState->reg_a2;

    char *s = sPtr->sup_exceptState->reg_a1; 
   
    if ((strlength >= 0) && (strlength <= MAXSTRLENG) && (*s >= UPROCSTARTADDR) && (*s <= USERSTACKTOP))
    {
    
        while ( *s != EOF)
        {
            //please create macro for this
            devReg->term.transm_command = (unsigned int) *s << 8;

            devReg->term.transm_command |= TRANSMITCHAR;

            SYSCALL(IOWAIT, 7, sPtr->sup_asid, 0);
            
            status = devReg->term.transm_status & 0xFF; // prendo lo status code in modo brutto
            
            if (status != OKCHARTRANS)
            {   
                sPtr->sup_exceptState->reg_v0 = status * -1; 
                break;
            }
            s++;
        }

        sPtr->sup_exceptState->reg_v0 = strlength; // VA TUTTO BENEEEE!!! :) ... e i bambini in africa ?
    }
    else
        Terminate_SYS9(sPtr);

    SYSCALL(VERHOGEN, (int)&support_wterminal_semaphore[sPtr->sup_asid - 1], 0 , 0);
}



void  Read_From_Terminal_SYS13(support_t* sPtr)
{
    devreg_t* devReg = DEV_REG_ADDR(7, sPtr->sup_asid); 

    unsigned int status;

    SYSCALL(PASSERN, (int) &support_rterminal_semaphore[sPtr->sup_asid - 1], 0, 0); 
    
    //indirizzo del buffer dove andiamo a scrivere
    char *buffer = sPtr->sup_exceptState->reg_a1; 

    int transmitted_char = 0;
   
    if ((buffer >= UPROCSTARTADDR) && (buffer <= USERSTACKTOP))
    {   
        //con questo comando il carattere viene messo in recv_status
        devReg->term.recv_command = TRANSMITCHAR;

        //ciclo finchè non incontro end of line
        while ( (devReg->term.recv_status >> 8) != EOL)
        {
            if ((buffer >= UPROCSTARTADDR) && (buffer <= USERSTACKTOP))
            {    
                SYSCALL(IOWAIT, 7, sPtr->sup_asid, 0);

                //prendo lo status value che sono solo i primi 8 bit
                status = devReg->term.recv_status & 0xFF;

                if (status != OKCHARTRANS)
                {   
                    sPtr->sup_exceptState->reg_v0 = status * -1; 
                    break;
                }
                // prendo il carattere shiftando di 8
                *buffer = devReg->term.recv_status >> 8;            

                buffer++;
                transmitted_char++;

                devReg->term.recv_command = TRANSMITCHAR;
            }
        }

        sPtr->sup_exceptState->reg_v0 = transmitted_char; 
    }
    else
        Terminate_SYS9(sPtr);

    SYSCALL(VERHOGEN, (int)&support_wterminal_semaphore[sPtr->sup_asid - 1], 0 , 0);
}