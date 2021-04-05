#ifndef EXCEPTION_HANDLER_H_INCLUDED
#define EXCEPTION_HANDLER_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "asl.h" 
#include "devinterrupt_handler.h"

/* The exception handler function of the kernel */
void exceptionHandler();

/*  PassUpOrDie procedure, in which we either pass the support information to the support level in order to
 *  properly handle the exception, or kill the process if the support info is null.
 */
void PassUpOrDie(int except_type, state_t* exceptionState);

#endif