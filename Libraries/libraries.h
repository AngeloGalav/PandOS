#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include "../Libraries/pandos_types.h"
#include "../Libraries/pandos_const.h"
#include "umps3/umps/libumps.h"

#define GET_BDP_STATUS(T) state_t* T = (state_t*) BIOSDATAPAGE

/* Sets the ExeCode of the exceptionsStatus to a value (used if syscall is called in usermode) */
void setExcCode(state_t* exceptionStatus, unsigned int toSet);

/* Sets the processor user-mode/kernel-mode bit and reloads the status */
void setProcessorMode(int mode);

/* Checks the KUp bit to see if we are in kernel mode */
int checkMode(unsigned int status_register);

/* Used as a substitute for stdlib's memcpy */
void *memcpy(void *dest, const void *src, size_t n);

unsigned int BitExtractor(unsigned int reg, int mask, int shift);

/* Returns the status word of a device register */
unsigned int GetStatusWord(int intlNo, int dnum, int waitForTermRead);

#endif