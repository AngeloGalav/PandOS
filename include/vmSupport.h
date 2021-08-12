#ifndef VMSUPPORT_H_INCLUDED
#define VMSUPPORT_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"
#include "definitions.h"
#include "libraries.h"
#include "debugger.h"

/* Initializes the swap table and swap pool semaphore */
void initSwapStructs();

/* Support level's pager. Handles page faults. */
void Support_Pager();

/* The pager's replacement algorithm: choses the page to drop located in a RAM frame, 
in order to free the frame. It returns the chosen page using a FIFO approach. */
int replacementAlgorithm();

/* The TLB refill handler: updates the cache in case of a tlb cache fault */
void uTLB_RefillHandler();

/** The flash device manager code.
 * @param command the command to send to the flash device. Can be read or write.
 * @param flash_asid the asid of the flash device, used to locate its address.
 * @param data_addr address of the frame to read/write onto the backing store.
 * @param deviceBlockNumber the number of the page to write into the backing store.
 *  
*/
void backStoreManager(unsigned int command, int flash_asid, unsigned int data_addr, unsigned int deviceBlockNumber);

// auxiliary function used by the syssupport when doing a sys9
void deleteSwTbEntries(int asid); 

#endif