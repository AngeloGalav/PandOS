#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include "syscall.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include "umps3/umps/libumps.h"

/* Sets the ExeCode of the exceptionsStatus to a value (used if syscall is called in usermode) */
void setExcCode(state_t* exceptionStatus, unsigned int toSet);

/* Checks the KUp bit to see if we are in kernel mode */
int checkMode(unsigned int status_register);

/* Used as a substitute for stdlib's memcpy */
void memcpy(void *dest, const void *src, size_t n);

/* a procedure to print on terminal 0 */
void print(char *msg);

unsigned int BitExtractor(unsigned int reg, int mask, int shift);

#endif