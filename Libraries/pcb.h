#ifndef PCB_H
#define PCB_H

#define MAXPROC 20
#define MAXINT 4294967295
#define HIDDEN static
#define TRUE            1
#define FALSE           0

#include "pandos_const.h"
#include "pandos_types.h"

void initPcbs();
void initializePcbt(pcb_t* node);
void freePcb(pcb_t* p);
pcb_t *allocPcb();

/// FUNZIONI DI MANIPOLAZIONE DELLE QUEUE ///

pcb_t* mkEmptyProcQ();
int emptyProcQ(pcb_t *tp);
void insertProcQ(pcb_t** tp, pcb_t* p);
pcb_t *headProcQ(pcb_t *tp);
pcb_t* removeProcQ(pcb_t **tp);
pcb_t* outProcQ(pcb_t **tp, pcb_t *p);

/// FUNZIONI SUGLI ALBERI ///

int emptyChild(pcb_t *p);
void insertChild(pcb_t *prnt, pcb_t *p);
pcb_t* removeChild(pcb_t *p);
pcb_t *outChild(pcb_t* p);

#endif
