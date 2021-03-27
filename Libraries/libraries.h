#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include "../Libraries/pandos_types.h"
#include "../Libraries/pandos_const.h"

/* Sets the ExeCode of the exceptionsStatus to a value (used if syscall is called in usermode) */
void setExeCode(state_t* exceptionStatus, unsigned int toSet);

/* Set the processor in kernel mode */
void setKernelMode(); //TODO

/* Set the processor in user mode */
void setUserMode(); //TODO

/* Checks the KUp bit to see if we are in kernel mode */
int checkMode(unsigned int status_register);

/* Used as a substitute for stdlib's memcpy */
void *memcpy(void *dest, const void *src, size_t n);

#endif