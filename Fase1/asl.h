#ifndef ASL_H
#define ASL_H
#include "pcb.h"

int insertBlocked(int *semAdd, pcb_t *p);
pcb_t* removeBlocked(int *semAdd);
pcb_t* outBlocked(pcb_t *p);
pcb_t* headBlocked(int *semAdd);
void initASL();

#endif // ASL_H
