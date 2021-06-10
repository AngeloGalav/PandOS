#include "../include/vmSupport.h"
#include "../include/definitions.h"

/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
int swap_semaphore; 


void initSwapStructs()
{
   
    swap_semaphore = 1;

    

}
