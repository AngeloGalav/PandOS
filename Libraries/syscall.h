#include "asl.h"
#include "pcb.h"

void *memcpy(void *dest, const void *src, size_t n);

HIDDEN void TerminateSingleProcess(pcb_t* to_terminate);

HIDDEN void KillRec(pcb_PTR root_child);

/* CreateProcess (SYS1)*/
void SYS1(state_t arg1, support_t* arg2);

void SYS2();

void SYS3(int** semAddr);

void SYS4(int** semAddr);

void SYS5(state_t * exceptionState);

void SYS8(state_t* exceptionState);