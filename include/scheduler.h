#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

#include "definitions.h"
#include "asl.h" 

/*  The scheduler function of the kernel.
 *  It dispatches a new process (if possible) and gives control to it.
 */
void Scheduler();

#endif