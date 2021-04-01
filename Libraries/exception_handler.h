#ifndef EXCEPTION_HANDLER_H_INCLUDED
#define EXCEPTION_HANDLER_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "asl.h" 
#include "devinterrupt_handler.h"

/**
 *  The exception handler function of the kernel
 *  Change this name 
*/
void exceptionHandler();

void PassUp(int except_type, state_t* exceptionState);

#endif