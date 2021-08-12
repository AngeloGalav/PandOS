#ifndef INITPROC_H_INCLUDED
#define INITPROC_H_INCLUDED

#include "pandos_const.h"
#include "pandos_types.h"
#include "libraries.h"

/* Initializes and creates a process with asid = id + 1 */
void initProcess(int id);

/* Initializes the Support data structures and the U-Processes, equivalent 
to the init process on UNIX operating systems */
void test();

/* Initializes the support struct device semaphores */
void initSemaphores();

#endif