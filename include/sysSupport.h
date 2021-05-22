#ifndef SYSSUPPORT_H_INCLUDED
#define SYSSUPPORT_H_INCLUDED

#include "pandos_const.h"
#include "definitions.h"


/* General exception handler for the Support level */
void GeneralException_Handler ();

/* The Support's level Syscall handler */
void Support_Syscall_Handler(state_t *exceptState);


#endif