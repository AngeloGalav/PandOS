#ifndef DEVINTERRUPT_HANDLER_H_INCLUDED
#define DEVINTERRUPT_HANDLER_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "scheduler.h"
#include "asl.h" 
#include "syscall.h"

/* Cycles and handles all pending interrupts from highest to lowest priority */
void getInterruptLine(unsigned int cause_reg);

/* Cycles and handles all internal pending interrupts from highest to lowest priority*/
void GeneralIntHandler(int line);

/* */
void NonTimerHandler();

#endif