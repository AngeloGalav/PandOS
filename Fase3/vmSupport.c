#include "../include/vmSupport.h"
#include "../include/definitions.h"

/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
int swap_semaphore; 


void initSwapStructs()
{
   
    swap_semaphore = 1;

    //initialize the swap table as empty in the beginning
    // every ASID should be set as -1
    // the start of the first page is calculated by overestimate the OS code size
    for (int i = 0 ; i < POOLSIZE ; i++)
    {
        swap_table[i].sw_asid = NOPROC ;
        swap_table[i].sw_pageNo = FLASHPOOLSTART + (PAGESIZE * i ); // every page is 4096 bytes after the other
        swap_table[i].sw_pte = NULL;
    }

}

void Support_Pager() // TLB_exception_Handler andrà richiamato immagino
{
     //We take the support info of the current process with the SYS8
    support_t *sPtr = SYSCALL(GETSPTPTR, 0, 0, 0);
    //extract the execCode to check which exception is happening dio caneporcodiomaiale
    int excCode = GET_EXEC_CODE(sPtr->sup_exceptState[0].cause);

    if (excCode == MOD) // se è una TLB modification exception la tratto come PROgram trap
        SYSCALL(TERMINATE,0 ,0, 0); 

    SYSCALL(VERHOGEN, (int) &swap_semaphore, 0, 0); // (cast a int inutile )?

    unsigned int p = sPtr->sup_exceptState[0].entry_hi >> VPNSHIFT;

    //replace algorithm, use a static variable that starts from 0 ( first in first out)
    //and we always increment it every time % pool size so it doens't overflow
    static int i = 0;
}

