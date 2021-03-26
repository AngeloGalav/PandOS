#include "../Libraries/pcb.h"
#include "../Libraries/asl.h"
#include "../Libraries/libraries.h"
#include "../Libraries/definitions.h"
#include "../Libraries/syscall.h"

extern pcb_PTR readyQueue;

extern pcb_PTR currentProcess;

extern unsigned int processCount;
extern unsigned int softBlockCount;
extern int device_semaphores[SEMAPHORE_QTY];

void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char*)dest)[i] = ((char*)src)[i];
    }
}

void SYS1(state_t arg1, support_t* arg2)
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
    /* Assuming the current process is the one that executes this functions */
    outChild(currentProcess); // 
    TerminateSingleProcess(currentProcess);
    KillRec(currentProcess->p_child);
}


HIDDEN void TerminateSingleProcess(pcb_t* to_terminate) // static because it can only be called in this file scope.
{
    // Se il valore del semaforo e' negativo, allora il numero dei processi bloccati corrisponde in quel semaforo corrisponde
    // al valore assoluto del valore del semaforo stesso. Questo valore, se un processo bloccato viene terminato, va aggiustato.
    processCount -= 1;
    freePcb(to_terminate);

    for (int i = 0; i < SEMAPHORE_QTY; i++)
    {
        if (to_terminate->p_semAdd == device_semaphores[i]) return; // Se e' un device semaphore, ho gia' fatto le mie operazioni
    }

    if (removeBlocked(to_terminate->p_semAdd) != NULL) // controllare se e' un device semaphore... (Come si fa?) 
    {
            *(to_terminate->p_semAdd) += 1;
            softBlockCount -= 1;
    }
}

/** Kills a process recursively
 * 
 * @param: the child of the root process we want to kill.
 */
HIDDEN void KillRec(pcb_PTR root_child)
{
    if (root_child == NULL) return;

    KillRec(root_child->p_next_sib);
    KillRec(root_child->p_child); // We repeat this recursevely for each child of the process
    TerminateSingleProcess(root_child);
}

void SYS3(int** semAddr) 
{
    *semAddr -= 1;
    if (*semAddr < 0) 
    {
        insertBlocked(semAddr, currentProcess); // if the value is negative, the process gets blocked
        softBlockCount += 1;     
    }
}

void SYS4(int** semAddr) 
{
    *semAddr += 1;
    removeBlocked(semAddr);
}

void SYS5()
{
    currentProcess->p_s.pc_epc += 4;
    
    state_t *exceptionState = (memaddr) BIOSDATAPAGE;
    currentProcess->p_s = *exceptionState;

    //update the accumulated CPU time

    SYS3(currentProcess->p_semAdd);

    //call scheduler
}

void SYS6()
{
    currentProcess->p_s.reg_v0 = currentProcess->p_time;
}

void SYS7() 
{
    SYS3(&device_semaphores[48]);
    //call scheduler
}

void SYS8() //Indecisione su quale registro salvare il dato, se nel current process oppure nell'exception state.
{
    currentProcess->p_s.reg_v0 = currentProcess->p_supportStruct; // DA CHIEDERE A MALDINI
}

void SyscallAccessDenied()
{
   // currentProcess->p_s.cause;
}

