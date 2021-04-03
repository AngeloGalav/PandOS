#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include "../Libraries/pandos_types.h"
#include "../Libraries/pandos_const.h"
#include "umps3/umps/libumps.h"

#define GET_BDP_STATUS(T) state_t* T = (state_t*) BIOSDATAPAGE


/* Used as a substitute for stdlib's memcpy */
void *memcpy(void *dest, const void *src, size_t n);

unsigned int BitExtractor(unsigned int reg, int mask, int shift);

#endif