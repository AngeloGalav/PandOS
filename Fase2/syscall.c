#include "libraries.h"

extern pcb_PTR readyQueue;

extern pcb_PTR currentProcess;

extern int processCount;

void SYS1(state_t arg1,support_t* arg2)
{
    pcb_PTR newproc = allocPcb(); 

    initializePcbt(newproc);
    insertProcQ(&(readyQueue), newproc);
    insertChild(currentProcess, newproc);

    processCount += 1;

    newproc->p_semAdd = NULL;
    newproc->p_time = 0; 
    newproc->p_s = arg1;
    if(arg2 != NULL)
        newproc->p_supportStruct = arg2;
    else    
        newproc->p_supportStruct = NULL;
}

void SYS2()
{
    pcb_PTR root_child;
    /* We return and eliminate the first child of the process tree */
    root_child = outChild(currentProcess->p_child);
    



}

void SYS8(state_t* arg1) //Indecisione su quale registro salvare il dato, se nel current process oppure nell'exception state.
{
    arg1->reg_v0 = currentProcess->p_supportStruct;
}
