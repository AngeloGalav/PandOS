#include "../include/vmSupport.h"

// current proc variable which is accessible through the uTLBRefillHandler, since it's a phase 2 function
extern pcb_PTR currentProcess;

/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphore*/
int swap_semaphore;

extern int flash_device_semaphores[UPROCMAX];

void initSwapStructs()
{
    swap_semaphore = 1;
    for (int i = 0; i < POOLSIZE; i++)
        swap_table[i].sw_asid = NOPROC;
}

void Support_Pager() // TLB_exception_Handler andrà richiamato immagino
{
    // Gathering the support info of the current process with the SYS8
    support_t *sPtr = (support_t*) SYSCALL(GETSPTPTR, 0, 0, 0);
    
    // extracting the execCode to check the exception type
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[PGFAULTEXCEPT].cause);
    
    if (excCode == EXC_MOD) // if the call is a TLB-Modification exception, then it's treated as a progrma trap (since it shouldnt happen)
        SYSCALL(TERMINATE, 0, 0, 0); 

    SYSCALL(PASSEREN, (memaddr) &swap_semaphore, 0, 0);

    // locate the missing page number (found in entryHI of the Saved Exception State)
    int p = (sPtr->sup_exceptState[PGFAULTEXCEPT].entry_hi - PAGETBLSTART) >> VPNSHIFT;

    if (p < 0 || p > 30) {bp(); p = MAXPAGES - 1;} // stack page detection
    
    // we look for a free frame...
    int victim_frame = -1;
    for (int i = 0; i < POOLSIZE; i++){
        if (swap_table[i].sw_asid == NOPROC){
            victim_frame = i;
            break;
        }
    }

    if (victim_frame == -1) victim_frame = replacementAlgorithm(); //... if there aren't any free frames, we call the replacement algorithm

    // the asid of the occupied frame
    int frame_asid = swap_table[victim_frame].sw_asid;
    
    // the asid of the process that caused the pagefault
    int asid = sPtr->sup_asid;

    // in case the page was occupied by a process, and we should assume that is dirty    
    if (frame_asid != NOPROC)
    {                          
        // DISABLING INTERRUPTS using getStatus -> setStatus
        DISABLE_INTERRUPTS_COMMAND;

        UNSET_BIT(swap_table[victim_frame].sw_pte->pte_entryLO, VALID_BIT_POS) ; //bisogna mettere il V bit a 0

        // tlb update
        TLBCLR();

        // ENABLING INTERRUPTS using setStatus
        ENABLE_INTERRUPTS_COMMAND;

        // address of the page to rewrite onto the flash device
        int page_addr = POOLSTART + (victim_frame * PAGESIZE);
        // block of the flash device to write to
        int block_number = swap_table[victim_frame].sw_pageNo;
        backStoreManager(FLASHWRITE, frame_asid, page_addr, block_number);
    }

    // frame address to write the data from flash device to ram
    memaddr frame_addr = POOLSTART + (victim_frame * PAGESIZE);

    bp_ignore();

    // copying the contentes of the page 'p' located into the backing store to the RAM frame.
    // essentially, a frame is identified by the asid and the page number.
    backStoreManager(FLASHREAD, asid, frame_addr, p);
    
    DISABLE_INTERRUPTS_COMMAND;
    
    /** STEP 10 **/
    // even though it is not specified, later in 4.5.3 it is said that the swap table must infact be updated atomically
    swap_table[victim_frame].sw_asid = asid;
    swap_table[victim_frame].sw_pageNo = p;
    swap_table[victim_frame].sw_pte = &(sPtr->sup_privatePgTbl[p]); // passing the address of the user's page to the swap_table

    /** STEP 11 **/
    //update the process page table
    sPtr->sup_privatePgTbl[p].pte_entryLO |= (victim_frame << PFNSHIFT) | VALIDON;
    // updating the current process page table entry
    swap_table[victim_frame].sw_pte->pte_entryLO = frame_addr | VALIDON | DIRTYON;

    bp_correct();

    /** STEP 12 **/
    //UPDATE TLB by clearing it. (after refactoring it will be more complex)
    TLBCLR();

    ENABLE_INTERRUPTS_COMMAND;

    /** STEP 13 **/
    // releasing the mutual exclusion semaphore over the Swap Pool table
    SYSCALL(VERHOGEN, (int) &swap_semaphore, 0, 0);

    /** STEP 14 **/
    // Return control to the support process by loading 
    LDST((state_t*) &(sPtr->sup_exceptState[PGFAULTEXCEPT]));
}

void backStoreManager(unsigned int command, int flash_asid, unsigned int data_addr, unsigned int deviceBlockNumber)
{
    SYSCALL(PASSEREN, (memaddr) &flash_device_semaphores[flash_asid - 1], 0, 0);
    devreg_t* devReg = GET_DEV_ADDR(FLASHINT, flash_asid - 1); // ASID values go from 1 to 8

    // block address of the data to read/write 
    devReg->dtp.data0 = data_addr;
    
    DISABLE_INTERRUPTS_COMMAND;
    // inserting the command after writing into data, since the IO operation happens just about when it is over
    devReg->dtp.command = (deviceBlockNumber << 8) | command; 
    
    SYSCALL(IOWAIT, FLASHINT, flash_asid - 1,0);

    ENABLE_INTERRUPTS_COMMAND;

    bp_correct();
    
    SYSCALL(VERHOGEN, (int) &flash_device_semaphores[flash_asid - 1], 0, 0);

    // treating any error as a Program Trap
    if (devReg->dtp.status != DEV0ON)
        SYSCALL(TERMINATE, 0, 0, 0); ///TODO: replace with sys9
}

int replacementAlgorithm()
{
    static int frame = 0; // this way we keep information in the local variable
    int old_frame = frame; 
    frame = ((frame + 1) % POOLSIZE);
    return old_frame;
}

void uTLB_RefillHandler() {
    
    // Get the page number
    int pageNumber;

    GET_BDP_STATUS(exception_state);
    pageNumber = (exception_state->entry_hi - PAGETBLSTART) >> VPNSHIFT; // getting the missing page number from the exception state in the BIOSDATAPAGE

    if (pageNumber < 0 || pageNumber > 30) {bp(); pageNumber = MAXPAGES - 1;} // stack page index.. another approch would have been to chek if vpn == 0xBFFFF
    
    // loading the page entry onto memory
    setENTRYHI(currentProcess->p_supportStruct->sup_privatePgTbl[pageNumber].pte_entryHI);  // REMEMBER: a process page table is different from the swap table
    setENTRYLO(currentProcess->p_supportStruct->sup_privatePgTbl[pageNumber].pte_entryLO);
    TLBWR();
    
    // Resuming normal control
    LDST(exception_state);
}