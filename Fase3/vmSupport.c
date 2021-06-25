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
#include "../include/definitions.h"


/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
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

/**
 * @brief Support level's pager. Handles page faults.
 * 
 */
void Support_Pager() // TLB_exception_Handler andrà richiamato immagino
{
    // We take the support info of the current process with the SYS8
    support_t *sPtr = SYSCALL(GETSPTPTR, 0, 0, 0);
    
    // extract the execCode to check the exception type
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);

    if (excCode == EXC_MOD) // if the call is a modification exception, then it's treated as a progrma trap (since it shouldnt happen)
        SYSCALL(TERMINATE,0 ,0, 0); 

    SYSCALL(PASSEREN, (memaddr) &swap_semaphore, 0, 0);

    // we locate the missing page info: 

    //Andare a prendere l'indirizz della page table address nel CP0 register nel BIOS
    //con quell'indirizzo dentro la page table del current process trovare l'iesima pagina
    //restituire l'indirizzo della pagina
    
    int p = Get_PageTable_Index(sPtr);

    // uso una variabile static che ogni volta andrà ad essere incrementata di uno % POOLSIZE
    // questo perchè quella che prendiamo poi viene "reinserita " quindi sarà la nuova pagina
    
    int frame = replacementAlgorithm(); ///!!!!!!!!!!!!!!!!!!!!!!!

    int asid = swap_table[frame].sw_asid ;
    //controllo se l'i-esima pagina scelta è valida controllandola dentro la spawpool_table
    
    if (asid != NOPROC)//OPPURE swapTable[frame].sw_pte->pte_entryLO & VALIDON)
    {   
        // DISABLING INTERRUPTS HERE using getStatus -> setStatus
        unsigned int entry = getSTATUS();
        UNSET_BIT(entry, IECON);// gliolevo 
        setSTATUS(entry);//gliorimetto
        
        //risalgo al process x dal frame k
        UNSET_BIT(swap_table[frame].sw_pte->pte_entryLO, VALIDON) ; //bisogna mettere il V bit a 0
     
        //UPDATE TLB
        //section 4.5.2 pandos
        TLBCLR(); // per il momento facciamo erase del TLB come consigliato, poi andrà cambiato

        // ENABLING INTERRUPTS HERE using setStatus
        setSTATUS(IECON);

        //UPDATE flash backing store
        devreg_t* devReg = DEV_REG_ADDR(4, asid - 1); // prendo il flash/backing store del process x
        //in DATA0 the starting physical address of the 4k block to be read (or written);
        // the particular frame’s starting address.
        devReg->dtp.data0 = swap_table[frame].sw_pte->pte_entryLO >> PFNSHIFT;
        devReg->dtp.command |= FLASHWRITE  | (frame << 8) ; 
        SYSCALL(IOWAIT,0,0,0);
        int status = devReg->dtp.status;
        if (status != DEV0ON)
            SYSCALL(TERMINATE,0,0,0); // program trap in the future
        //check the status and see if an error occurred, if yes generate a program trap
         
         //read from the flash device
        devReg->dtp.command |= READBLK | (frame << 8) ;
        SYSCALL(IOWAIT,0,0,0);
        memaddr* address = devReg->dtp.data0; // prendo l'indirizzo dentro data0
        swap_table[frame].sw_pte->pte_entryLO = *address; // i don't know

        //DISABLE INTERRUPTS

        swap_table[frame].sw_asid = asid;
        swap_table[frame].sw_pageNo = p;
        swap_table[frame].sw_pte = &(sPtr->sup_privatePgTbl[p]);


        //update the process page table
         sPtr->sup_privatePgTbl[p].pte_entryLO |= VALIDON;
         sPtr->sup_privatePgTbl[p].pte_entryLO |= (frame << PFNSHIFT) ;

         //UPDATE TLB
        //section 4.5.2 pandos
        TLBCLR();

        //ENABLE INTERRUPTS

        SYSCALL(VERHOGEN, (memaddr) &swap_semaphore,0,0);

        LDST(sPtr->sup_exceptState);
<<<<<<< HEAD
=======

        
>>>>>>> 78022a12474adf3fb247a88a944e2c9739234c2a
    }
}

/**
 * @brief calculates the missing page which caused a page fault 
 * 
 * @param sPtr, support structure pointer
 * @return the page table index
 */
int Get_PageTable_Index(support_t* sPtr)
{
    //vado a vedere nella memoria logica del processo quale pagina corrisponde 
    //così mi rendo conto quale era quella che cercava e che ha scatenato il fault
    GET_BDP_STATUS(BadVADDR);
    unsigned int BadVaddr = BadVADDR->gpr[CP0_BadVAddr]; 
    for(int i = 0; i < MAXPAGES ; i++)
    {
        if((sPtr->sup_privatePgTbl[i].pte_entryHI >> VPNSHIFT) == BadVaddr)
            return i; //ritorniamo l'indice della pagina a cui corrisponde l'address nell'entryHi
    }
}

/**
 * @brief return the frame index in the RAM that is going to be replaced by a new page,
 * using FIFO. 
 * 
 * @return int 
 */
int replacementAlgorithm()
{
    static int frame = 0; // this way we keep information in the local variable
    int old_frame = frame; 
    frame = (frame % POOLSIZE) + 1;
    return old_frame;
}

/**
 * @brief TLB refill handler
 * 
 */
void uTLB_RefillHandler() {
    // Get the page number
    volatile unsigned int pageNumber;
    //non sono sicuro se bisogna controllare tutto l'entryHi per vedere se è compreso in una
    //area di memoria o basta anche solo il VPN 
    //pageNumber = sPtr->sup:exceptState->entryhi >> VPNSHIFT
    //if è nello stack quindi tra UPROCSTACKPG e  USERSTACKTOP allora la page number è 31
    //altrimenti 
    // pageNumber = pageNumber - VPNBASE ( dovrebbe essere 0x80000)
    pageNumber = GETVPN(EXCSTATE->entry_hi);

    //trovo la entry dentro la page table del processo ?
    pteEntry_t *entry = findEntry(pageNumber);

    
    setENTRYHI(entry->pte_entryHI);
    setENTRYLO(entry->pte_entryLO);
    TLBWR();

    // Return control to the process by loading the processor state
    resume();
}