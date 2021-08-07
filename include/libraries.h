#ifndef LIBRARIES_H_INCLUDED
#define LIBRARIES_H_INCLUDED

#include "syscall.h"
#include "pandos_types.h"
#include "pandos_const.h"
#include <umps3/umps/libumps.h>

#define size_t unsigned int

/* Used as a substitute for stdlib's memcpy */
void memcpy(void *dest, const void *src, size_t n);

/* Exctracts a certain portion of bits of an unsigned int */
unsigned int BitExtractor(unsigned int reg, int mask, int shift);

#endif