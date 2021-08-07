/**
 * @file vmSupport.c
 * 
 * @brief this module should contain the TLB exception handler, and
 * the pager. 
 * it also contains the swap table and the swap pool semaphore 
 * 
 * @version 0.1
 * 
 */

#include "../include/vmSupport.h"

// current proc variable which is accessible through the uTLBRefillHandler, since it's a phase 2 function
extern pcb_PTR currentProcess;


/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphore*/
int swap_semaphore; 

/**
 * @brief initializes the swap table and swap pool semaphore
 * 
 */
void initSwapStructs()
{
    swap_semaphore = 1;
    for (int i = 0; i < POOLSIZE; i++)
        swap_table[i].sw_asid = NOPROC;
}

void Support_Pager() // TLB_exception_Handler andrà richiamato immagino
{
    // We take the support info of the current process with the SYS8
    support_t *sPtr = (support_t*) SYSCALL(GETSPTPTR, 0, 0, 0);
    
    // extract the execCode to check the exception type
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);

    if (excCode == EXC_MOD) // if the call is a modification exception, then it's treated as a progrma trap (since it shouldnt happen)
        SYSCALL(TERMINATE,0 ,0, 0); 

    SYSCALL(PASSEREN, (memaddr) &swap_semaphore, 0, 0);

    // we locate the missing page info: 

    // Andare a prendere l'indirizz della page table address nel CP0 register nel BIOS
    // con quell'indirizzo dentro la page table del current process trovare l'iesima pagina
    // restituire l'indirizzo della pagina
    
    int p = Get_PageTable_Index(sPtr); // locate the missing page number (found in entryHI of the Saved Exception State)
    
    int frame = replacementAlgorithm(); // we are chosing the frame to replace

    int asid = swap_table[frame].sw_asid;

    devreg_t* devReg = NULL;

    // in case the page was occupied by a process, and we should assume that is dirty    
    if (asid != NOPROC)     //OPPURE swapTable[frame].sw_pte->pte_entryLO & VALIDON)
    {                          
        // DISABLING INTERRUPTS HERE using getStatus -> setStatus
        DISABLE_INTERRUPTS_COMMAND;

        //risalgo al process x dal frame k
        UNSET_BIT(swap_table[frame].sw_pte->pte_entryLO, VALID_BIT_POS) ; //bisogna mettere il V bit a 0

        //UPDATE TLB (section 4.5.2 pandos)
        TLBCLR(); // per il momento facciamo erase del TLB come consigliato, poi andrà cambiato

        // ENABLING INTERRUPTS HERE using setStatus
        ENABLE_INTERRUPTS_COMMAND;

        // UPDATE flash backing store
        devReg = (devreg_t*) DEV_REG_ADDR(4, asid - 1); // prendo il flash/backing store del process x
        
        // writing the starting physical address of the 4k block in DATA0 as we should
        devReg->dtp.data0 = swap_table[frame].sw_pte->pte_entryLO >> PFNSHIFT;
        
        // inserting the command that indicates a write operations
        devReg->dtp.command |= FLASHWRITE  | (frame << 8) ; 
        SYSCALL(IOWAIT,0,0,0);

    } // } --- i assumed that steps 8.a, 8.b and 8.c should be done in case the frame is occupied 

    // in case the frame wasnt occupied, i should assign it a value since we skipped the previous steps
    if (devReg == NULL) devReg = (devreg_t*) DEV_REG_ADDR(4, asid - 1);

    /** STEP 9 **/
    int status = devReg->dtp.status;
    if (status != DEV0ON)
        SYSCALL(TERMINATE,0,0,0); // we treat an error as a program trap
    // check the status and see if an error occurred, if yes generate a program trap
         
    //read from the flash device
    devReg->dtp.command |= READBLK | (frame << 8) ;
    SYSCALL(IOWAIT,0,0,0);
    
    //memaddr* address = (memaddr*) devReg->dtp.data0; // prendo l'indirizzo dentro data0
    //swap_table[frame].sw_pte->pte_entryLO = *address; // i don't know //** questa riga non ha senso contando che comunque dopo rimpiazzi totalmente la pagina.
                                                        // inoltre, causa un segfault

    DISABLE_INTERRUPTS_COMMAND;

    /** STEP 10 **/
    swap_table[frame].sw_asid = asid;
    swap_table[frame].sw_pageNo = p;
    swap_table[frame].sw_pte = &(sPtr->sup_privatePgTbl[p]); // passing the address of the user's page to the swap_table

    /** STEP 11 **/
    //update the process page table
    sPtr->sup_privatePgTbl[p].pte_entryLO |= VALIDON;
    sPtr->sup_privatePgTbl[p].pte_entryLO &= 0x7FF; // we clean the PFN string
    sPtr->sup_privatePgTbl[p].pte_entryLO |= (frame << PFNSHIFT) ;

    /** STEP 12 **/
    //UPDATE TLB by clearing it. (after refactoring it will be more complex)
    TLBCLR();

    ENABLE_INTERRUPTS_COMMAND;

    /** STEP 13 **/
    // releasing the mutual exclusion semaphore over the Swap Pool table
    SYSCALL(VERHOGEN, (memaddr) &swap_semaphore, 0, 0);

    /** STEP 14 **/
    // Return control to the support process by loading 
    LDST(sPtr->sup_exceptState);
    
    // } --- i assumed that steps 8.a, 8.b and 8.c should be done in case the frame is occupied 
}

int Get_PageTable_Index(support_t* sPtr)
{
    // vado a vedere nella memoria logica del processo quale pagina corrisponde 
    // così mi rendo conto quale era quella che cercava e che ha scatenato il fault
    GET_BDP_STATUS(exception_status);
    /* unsigned int BadVaddr = exception_status->gpr[CP0_BadVAddr]; 
    for (int i = 0; i < MAXPAGES; i++)
    {
        if ((sPtr->sup_privatePgTbl[i].pte_entryHI >> VPNSHIFT) == BadVaddr)
            return i; //ritorniamo l'indice della pagina a cui corrisponde l'address nell'entryHi
    } */

    return exception_status->entry_hi >> VPNSHIFT; // ritorna la virtual page number
}

int replacementAlgorithm()
{
    static int frame = 0; // this way we keep information in the local variable
    int old_frame = frame; 
    frame = (frame % POOLSIZE) + 1;
    return old_frame;
}

void uTLB_RefillHandler() {
    
    // Get the page number
    volatile unsigned int pageNumber;

    GET_BDP_STATUS(exception_state);
    pageNumber = exception_state->entry_hi >> VPNSHIFT; // getting the missing page number from the exception state in the BIOSDATAPAGE

    pteEntry_t entry;
    entry.pte_entryHI = MAXINT;

    // Locating the correct page table entry from the currentProcess 
    for (int i = 0; i < 32; i++){
        if ((currentProcess->p_supportStruct->sup_privatePgTbl[i].pte_entryHI >> VPNSHIFT) == pageNumber)
            entry = currentProcess->p_supportStruct->sup_privatePgTbl[i];
    }

    if (entry.pte_entryHI == MAXINT) bp_PAGE_NOT_FOUND(); // this will be an error status in case the wrong page still wasnt found
    
    setENTRYHI(entry.pte_entryHI);
    setENTRYLO(entry.pte_entryLO);
    TLBWR();
    
    // Resuming normal control
    LDST((state_t*) BIOSDATAPAGE);
}