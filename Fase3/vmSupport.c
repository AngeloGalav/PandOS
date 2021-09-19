#include "../include/vmSupport.h"

// current proc variable which is accessible through the uTLBRefillHandler, since it's a phase 2 function
extern pcb_PTR currentProcess;

/* Swap pool table: describes the current situation of the swap pool frames in RAM */
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

void Support_Pager()
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

    if (p < 0 || p > 30) 
        p = MAXPAGES - 1; // stack page detection
    
    // we look for a free frame...
    int victim_frame = -1;
    for (int i = 0; i < POOLSIZE; i++){
        if (swap_table[i].sw_asid == NOPROC){
            victim_frame = i;
            break;
        }
    }

    // the replacentAlgorithm returns the index of an occupied frame that we want to dispose.
    if (victim_frame == -1) victim_frame = replacementAlgorithm(); //... if there aren't any free frames, we call the replacement algorithm

    // the asid of the occupied frame
    int frame_asid = swap_table[victim_frame].sw_asid;
    
    // the asid of the process that caused the pagefault
    int asid = sPtr->sup_asid;

    // frame address to write the data from flash device to ram
    memaddr frame_addr = POOLSTART + (victim_frame * PAGESIZE);

    // in case the page was occupied by a process, and we should assume that is dirty    
    if (frame_asid != NOPROC)
    {                          
        // DISABLING INTERRUPTS using getStatus -> setStatus
        DISABLE_INTERRUPTS_COMMAND;

        UNSET_BIT(swap_table[victim_frame].sw_pte->pte_entryLO, VALID_BIT_POS); // setting the V bit to 0

        // updating the single TLB entry
        updateTLB(swap_table[victim_frame].sw_pte); // we need to update the single page, that was modified in the previous instruction
                                                    // (it's modified since the valid bit is set to off).

        // ENABLING INTERRUPTS using setStatus
        ENABLE_INTERRUPTS_COMMAND;
        
        // block of the flash device to write to (coincides with the page number)
        int block_number = (swap_table[victim_frame].sw_pte->pte_entryHI - PAGETBLSTART) >> VPNSHIFT;

        // writing contents of frame (stored in frame_addr) into the backing store in the block_number
        backStoreManager(FLASHWRITE, frame_asid, frame_addr, block_number);
    }

    // copying the contentes of the page 'p' located into the backing store to the RAM frame.
    // essentially, a frame is identified by the asid and the page number.
    backStoreManager(FLASHREAD, asid, frame_addr, p);
    
    DISABLE_INTERRUPTS_COMMAND;
    
    // Even though it is not specified, later in 4.5.3 it is said that the swap table must infact be updated atomically
    swap_table[victim_frame].sw_asid = asid;
    swap_table[victim_frame].sw_pageNo = p;
    swap_table[victim_frame].sw_pte = &(sPtr->sup_privatePgTbl[p]); // passing the address of the user's page to the swap_table

    // Updating the process page table

    // PFN == INDEX OF FRAME IN RAM !! (=! physical address of anything)
    sPtr->sup_privatePgTbl[p].pte_entryLO = (frame_addr & 0xFFFFF000) | VALIDON | DIRTYON; // dirty bit means the page is write enabled

    // Updating the TLB entry of the page that was just inserted into RAM.
    updateTLB(&(sPtr->sup_privatePgTbl[p]));

    ENABLE_INTERRUPTS_COMMAND;

    // releasing the mutual exclusion semaphore over the Swap Pool table
    SYSCALL(VERHOGEN, (int) &swap_semaphore, 0, 0);

    // Return control to the support process by loading 
    LDST((state_t*) &(sPtr->sup_exceptState[PGFAULTEXCEPT]));
}

void backStoreManager(unsigned int command, int flash_asid, unsigned int data_addr, unsigned int deviceBlockNumber)
{
    // we gain mutual exclusion on the flash device.
    SYSCALL(PASSEREN, (memaddr) &flash_device_semaphores[flash_asid - 1], 0, 0);
    devreg_t* devReg = GET_DEV_ADDR(FLASHINT, flash_asid - 1); // ASID values go from 1 to 8

    // block address of the data to read/write 
    devReg->dtp.data0 = data_addr;
    
    DISABLE_INTERRUPTS_COMMAND;
    // inserting the command after writing into data, since the IO operation happens just about when it is over
    devReg->dtp.command = (deviceBlockNumber << 8) | command; 
    
    SYSCALL(IOWAIT, FLASHINT, flash_asid - 1,0);

    ENABLE_INTERRUPTS_COMMAND;
    
    SYSCALL(VERHOGEN, (int) &flash_device_semaphores[flash_asid - 1], 0, 0);

    // treating any error as a Program Trap
    if (devReg->dtp.status != DEV0ON)
        SYSCALL(TERMINATE, 0, 0, 0);
}

void updateTLB(pteEntry_t *updated_entry)
{   
    setENTRYHI(updated_entry->pte_entryHI);
    TLBP(); // examine the TLB to search if the TLB entry is in the TLB.
            // TLBP() searches for a TLB entry that matches the current values of the entryHI register in the CPU. 
            // The return value of the probing is then place into the Index register of the CPU.

    if((getINDEX() & PRESENTFLAG) == 0) // If TLB entry is present, returns 0 into the P value of the Index reg.
    {   
        setENTRYLO(updated_entry->pte_entryLO); // If it's present, we update the entry. 
        TLBWI(); // EntryHI is already set, so we can write the whole pte into the TLB.
        // TLBWI writes using the information in the Index register.
    } 
}

int replacementAlgorithm()
{
    static int frame = 0; // this way we keep information in the local variable
    int old_frame = frame; 
    frame = ((frame + 1) % POOLSIZE);
    return old_frame;
}

void uTLB_RefillHandler() {

    GET_BDP_STATUS(exception_state); // retrieving the exception state
    int pageNumber = (exception_state->entry_hi - PAGETBLSTART) >> VPNSHIFT; // getting the missing page number from the exception state in the BIOSDATAPAGE

    if (pageNumber < 0 || pageNumber > 30) 
        pageNumber = MAXPAGES - 1; // stack page index.. another approch would have been to chek if vpn == 0xBFFFF
    
    // loading the page entry onto memory
    setENTRYHI(currentProcess->p_supportStruct->sup_privatePgTbl[pageNumber].pte_entryHI);  // REMEMBER: a process page table is different from the swap table
    setENTRYLO(currentProcess->p_supportStruct->sup_privatePgTbl[pageNumber].pte_entryLO);
    TLBWR();
    
    // Resuming normal control
    LDST(exception_state);
}