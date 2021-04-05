#ifndef DEVINTERRUPT_HANDLER_H_INCLUDED
#define DEVINTERRUPT_HANDLER_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "scheduler.h"
#include "asl.h" 
#include "definitions.h"
#include "syscall.h"

/* Cycles and handles all pending interrupts from highest to lowest priority */
void InterruptHandler(unsigned int cause_reg);

/* Cycles and handles all internal pending interrupts from highest to lowest priority*/
void InterruptLineHandler(int line);

/* Handles the non-timer device interrupts */
void NonTimerHandler();

#endif