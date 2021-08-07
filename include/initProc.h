#ifndef INITPROC_H_INCLUDED
#define INITPROC_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"
#include "libraries.h"

/* Initialize the support structs for the current process */
extern void initSupportStructs(); // extern cause it should be in TLB file

/* Initializes the Support data structures and the U-Processes */
void test();

/* Used to invoke SYS1 for each process */
void UProcInitiliazer();

#endif