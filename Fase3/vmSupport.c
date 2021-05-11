#include "../include/vmSupport.h"

/* Swap pool data structures */
swap_t swap_pool_table[UPROCMAX];
int swap_pool_semaphore;

void initSwapStructs()
{
    swap_pool_semaphore = 1; /* it's a mutual exclusion semaphore */


}
