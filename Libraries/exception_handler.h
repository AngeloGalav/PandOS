#ifndef EXCEPTION_HANDLER_H_INCLUDED
#define EXCEPTION_HANDLER_H_INCLUDED

#include "definitions.h"
#include "asl.h" 

/* The exception handler function of the kernel */
void fooBar();

/*  */
HIDDEN void SyscallAccessDenied();

#endif