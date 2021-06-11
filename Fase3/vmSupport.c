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
