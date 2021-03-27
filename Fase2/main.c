#include "pandos_const.h"
#include "pandos_types.h"
#include "umps3/umps/aout.h"
#include "umps3/umps/arch.h"
#include "umps3/umps/bios_defs.h"
#include "umps3/umps/cp0.h"
#include "umps3/umps/regdef.h"
#include <umps3/umps/libumps.h>
#include "asl.h"
#include "pcb.h"

/* Define the 100000 milliseconds */
#define TIMERVALUE  (PSECOND / * ((unsigned int*) TIMESCALEADDR))
#define DEVICES_NUMBER 17
/* Include the test function */
extern void test();

/* Number of started, but not yet terminated processes. */
unsigned int processCount = 0;

/* Number of started, but not terminated 
processes that in are the “blocked” state 
due to an I/O or timer request */
unsigned int softBlockCount = 0;

/* Tail pointer to a queue of pcbs that are in the “ready” state. */
pcb_PTR readyQueue;

/* Pointer to the current active process in "ready" state */ 
pcb_PTR currentProcess;

//TO-DO capire come inizializzare i semaphores devices

/* Inizialize pass-up-vector with the addressess needed */
passupvector_t* passupvector ;

/* Int Array for semaphores*/
int semaphores[DEVICES_NUMBER];

int main()
{

    readyQueue = mkEmptyProcQ();
    initPcbs(); 
    initASL();

    /* Fill up pass-up-vector*/
    passupvector->tlb_refill_handler = (memaddr) uTLB_RefillHandler() ; //Codice già dato nel p2test.c
    passupvector->tlb_refill_stackPtr = (memaddr) 0x20001000;
    passupvector->exception_stackPtr = (memaddr) 0x20001000 ;
    passupvector->exception_handler = (memaddr) fooBar(); //fooBar is for exception

    if(!getSTATUS()) // check the kernel mode ?????
        LDIT(TIMERVALUE);

    /* Start the process initialization */
    pcb_PTR proc = allocPcb() ;
    
    insertProcQ(&(readyQueue), proc);
    
    processCount += 1;

    initializePcbt(proc);
    proc->p_time = 0;
    proc->p_semAdd = NULL;
    proc->p_supportStruct = NULL;
    // nel 28* registro del GPR c'è lo stack pointer del programma, dobbiamo scrivere li dentro il valore di RAMTOP
    //((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE))))
    proc->p_s.gpr[28] = ((*((int *)RAMBASEADDR)) + (*((int *)RAMBASESIZE)));

    //PC
    proc->p_s.pc_epc = (memaddr) test; 

    // 00001000000000000000000000001001 == 134217737 passare un numero che mette i bit dello status register come vogliamo?
    proc->p_s.status = setSTATUS(134217737);

}