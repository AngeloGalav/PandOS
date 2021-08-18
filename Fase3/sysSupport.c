#include "../include/sysSupport.h"
#include "../include/syscall.h"
#include "../include/libraries.h"

extern int masterSemaphore;
extern int support_devsemaphores[SUPP_SEM_N][UPROCMAX];
extern int flash_device_semaphores[UPROCMAX];
extern swap_t swap_table[POOLSIZE];

void bp_print_fail(){};
void bp_invalid_exec(){};

void GeneralException_Handler()
{
    // We take the support info of the current process with the SYS8
    support_t *support_ptr = (support_t*) SYSCALL(GETSPTPTR, 0, 0, 0);
    // extract the execCode to check if there is a SYSCALL or a TRAP exception
    int excCode = GET_EXEC_CODE(support_ptr->sup_exceptState[GENERALEXCEPT].cause);
    
    if (excCode >= 9)
        Terminate_SYS9(support_ptr);

    int sysnumber = (int) support_ptr->sup_exceptState[GENERALEXCEPT].reg_a0;
    support_ptr->sup_exceptState[GENERALEXCEPT].pc_epc += WORDLEN;

    switch (sysnumber)
    {
        case TERMINATE:
            Terminate_SYS9(support_ptr);
            break;

        case GET_TOD:
            Get_Tod_SYS10(support_ptr);
            break;

        case WRITEPRINTER:
            Write_To_Printer_SYS11(support_ptr);
            break;

        case WRITETERMINAL:
            Write_to_Terminal_SYS12(support_ptr);
            break;

        case READTERMINAL:
            Read_From_Terminal_SYS13(support_ptr);
            break;

        default:
            Terminate_SYS9(support_ptr);
            break;
    }

    LDST((state_t*) &(support_ptr->sup_exceptState[GENERALEXCEPT]));
}   

void Terminate_SYS9(support_t* sPtr) // sys2 wrapper 
{  
    // check if the process to be terminated is holding any mutex semaphore
    // in that case use a SYS4 to free it and terminate it with sys2

    deleteSwTbEntries(sPtr->sup_asid);
    
    for (int i = 0; i < SUPP_SEM_N; i++) 
    {
        if (support_devsemaphores[i][sPtr->sup_asid - 1] <= 0)
            SYSCALL(VERHOGEN, (int) &support_devsemaphores[i][sPtr->sup_asid - 1], 0, 0); 
    }

    if (flash_device_semaphores[sPtr->sup_asid - 1] <= 0)
        SYSCALL(VERHOGEN, (int) &flash_device_semaphores[sPtr->sup_asid - 1], 0, 0); 

    // waking up the test proc
    SYSCALL(VERHOGEN, (int) &masterSemaphore, 0, 0);

    SYSCALL(TERMPROCESS, 0, 0, 0);  
}

void Get_Tod_SYS10(support_t* sPtr) 
{
    unsigned int tod;
    STCK(tod); // macro that takes unsigned int and populates it with TOD value
    sPtr->sup_exceptState[GENERALEXCEPT].reg_v0 = (unsigned int) tod;
}

void Write_To_Printer_SYS11(support_t* sPtr)
{
    int n_char_sent = 0;

    // Printer device associated with the U-proc 
    devreg_t* devReg = GET_DEV_ADDR(PRNTINT, sPtr->sup_asid - 1); // prende indirizzo del device register
    unsigned int status;
    
    SYSCALL(PASSERN, (int) &support_devsemaphores[PRINTER][sPtr->sup_asid - 1], 0, 0); 
    
    int strlength = (int) sPtr->sup_exceptState[GENERALEXCEPT].reg_a2;
    char *s = (char*) sPtr->sup_exceptState[GENERALEXCEPT].reg_a1; 
    
    // check if string address are between user space
    if ((strlength >= 0) && (strlength <= MAXSTRLENG) && (int) s >= KUSEG)
    {
        while (n_char_sent < (int) strlength)
        {
            // in printers, characters must be written in the lower end bytes of DATA0
            devReg->dtp.data0 = *s;

            DISABLE_INTERRUPTS_COMMAND;
            devReg->dtp.command = PRINTCHR;
            SYSCALL(IOWAIT, PRNTINT, sPtr->sup_asid - 1, 0);
            ENABLE_INTERRUPTS_COMMAND;

            status = devReg->dtp.status; // in printer, we can only use device ready as our OK code...
            
            if (status != 1)
            {   
                n_char_sent = status * -1; 
                break;
            }
            s++;
            n_char_sent++;
        }
    }
    else
        Terminate_SYS9(sPtr);
        
    
    sPtr->sup_exceptState[GENERALEXCEPT].reg_v0 = n_char_sent; // VA TUTTO BENEEEE!!! :) ... e i bambini in africa ?
    SYSCALL(VERHOGEN, (int) &support_devsemaphores[PRINTER][sPtr->sup_asid - 1], 0 , 0);
}


void Write_to_Terminal_SYS12(support_t* sPtr)
{
    int n_char_sent = 0;

    // device addr associated with the terminal
    devreg_t* devReg = GET_DEV_ADDR(TERMINT, sPtr->sup_asid - 1); // prende indirizzo del device register
    unsigned int status;

    SYSCALL(PASSERN, (memaddr) &support_devsemaphores[WRITETERM][sPtr->sup_asid - 1], 0, 0); 
    
    int strlength = (int) sPtr->sup_exceptState[GENERALEXCEPT].reg_a2;
    char *s = (char*) sPtr->sup_exceptState[GENERALEXCEPT].reg_a1; 
   
    if ((strlength >= 0) && (strlength <= MAXSTRLENG) && (memaddr) s >= KUSEG)
    {
        while (n_char_sent < (int) strlength)
        {
            DISABLE_INTERRUPTS_COMMAND;
            devReg->term.transm_command = (unsigned int) *s << 8 | TRANSMITCHAR;
            status = SYSCALL(IOWAIT, TERMINT, sPtr->sup_asid - 1, 0); // the interrupt handler returns the status BEFORE the ACK,
            ENABLE_INTERRUPTS_COMMAND;                                // which is what we need (not after like term.trasm_status)
            
            status &= 0xFF; // gathering status code from first byte
            
            if ((status & 0xFF) != OKCHARTRANS)
            {   
                n_char_sent = status * -1; 
                break;
            }
            s++;
            n_char_sent++;
        }
    }
    else
        Terminate_SYS9(sPtr);

    sPtr->sup_exceptState[GENERALEXCEPT].reg_v0 = n_char_sent; // VA TUTTO BENEEEE!!! :) ... e i bambini in africa ?
    SYSCALL(VERHOGEN, (int)&support_devsemaphores[WRITETERM][sPtr->sup_asid - 1], 0 , 0);
}


void Read_From_Terminal_SYS13(support_t* sPtr)
{
    devreg_t* devReg = GET_DEV_ADDR(TERMINT, sPtr->sup_asid - 1); 

    unsigned int status;

    SYSCALL(PASSERN, (int) &support_devsemaphores[READTERM][sPtr->sup_asid - 1], 0, 0); 
    
    // buffer address
    char *buffer = (char*) sPtr->sup_exceptState[GENERALEXCEPT].reg_a1; 

    int n_char_sent = 0;
   
    if ((memaddr) buffer >= KUSEG)
    {   
        while (TRUE)
        {
            DISABLE_INTERRUPTS_COMMAND;
            devReg->term.recv_command = TRANSMITCHAR;
            status = SYSCALL(IOWAIT, TERMINT, sPtr->sup_asid - 1, 1); // remember, termRead!! 
            ENABLE_INTERRUPTS_COMMAND;                                // arg3 must be 1 !!

            if ((status & 0xFF) == OKCHARTRANS)
            {
                n_char_sent++;
                *buffer = (status >> 8) & 0xFF;
                char latest_char = *buffer;
                buffer++;

                if (latest_char == '\n') break;
            }
            else
            {
                n_char_sent = ((status >> 8) & 0xFF) * -1;
                break;
            }
        }
    }
    else
        Terminate_SYS9(sPtr);

    sPtr->sup_exceptState[GENERALEXCEPT].reg_v0 = n_char_sent; 
    SYSCALL(VERHOGEN, (int)&support_devsemaphores[READTERM][sPtr->sup_asid - 1], 0 , 0);
}

void deleteSwTbEntries(int asid)
{
    for (int i = 0; i < POOLSIZE; i++)
    {
        if (swap_table[i].sw_asid == asid)
            swap_table[i].sw_asid = NOPROC;
    }
}