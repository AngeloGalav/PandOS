#ifndef VMSUPPORT_H_INCLUDED
#define VMSUPPORT_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"
#include "definitions.h"
#include "libraries.h"
#include "debugger.h"

/**/
void initSwapStructs();

/*  Support level's pager. Handles page faults. */
void Support_Pager();

/* Calculates the missing page which caused a page fault */
int Get_PageTable_Index(support_t* sPtr);

/* The pager's replacement algorithm: choses the page to drop located in a RAM frame, 
in order to free the frame. It returns the chosen page using a FIFO approach. */
int replacementAlgorithm();

/* The TLB refill handler: updates the cache in case of a tlb cache fault */
void uTLB_RefillHandler();

#endif