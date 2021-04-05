#include "../include/asl.h"

// array of inactive semaphores
HIDDEN semd_t semd_table[MAXPROC + 2];

// pointer to the free semaphores list not being used
HIDDEN semd_t* semdFree_h;

// pointer to the semaphores list currently being used
HIDDEN semd_t* semd_h;


/*
    The PCB pointed by p gets queued up in the blocked process queue paired with SEMD with 
    a semAdd key (?). If the corresponding semaphore isn't in the ASL, a new SEMD gets allocated
    from the list of free SEMDs (semdFree), setting the appropriate fields (i.e. key and s_procQ).
    A TRUE statement is returned if a new SEMD can't be allocated from the free SEMD list.
    In any other case a FALSE statement is returned.  
*/
int insertBlocked(int *semAdd, pcb_t *p)
{
    semd_t* cursor = semd_h;    // here's our first dummy node

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        if (cursor->s_semAdd == semAdd)
        {
            insertProcQ(&(cursor->s_procQ), p);
            p->p_semAdd = semAdd;
            return FALSE;
        }
        // Viene effettuato l'inserimento rispettando un ordine non decrescente (?)
        // The list is populated in a nondecreasing way
        else if (cursor->s_next->s_semAdd > semAdd || cursor->s_next->s_semAdd == (int*)MAXINT)
        {
            if (semdFree_h == NULL)
                return TRUE;
            else
            {
                semd_t* toAdd = semdFree_h; // we assign from the free descriptor list
                semdFree_h = semdFree_h->s_next;
                toAdd->s_next = cursor->s_next;
                cursor->s_next = toAdd;
                toAdd->s_procQ = mkEmptyProcQ();
                insertProcQ(&(toAdd->s_procQ), p);
                
                // the address of the descriptor gets updated
                toAdd->s_semAdd = semAdd;
                
                p->p_semAdd = semAdd;
                return FALSE;
            }
        }
        cursor = cursor->s_next;
    }
    return FALSE;
}



/*
    Returns the first PCB from the blocked process queue (s_procq) associated to 
    the ASL SEMD with semAdd key. If the descriptor is not in the ASL, NULL is returned,
    if it is, the removed element is returned. If the semaphore's blocked process queue is emptied,
    the corresponding descriptor is removed and placed from the ASL to
    the free descriptor queue (semdFree_h).
*/
pcb_t* removeBlocked(int *semAdd)
{
    semd_t* cursor = semd_h->s_next;
    semd_t* cursor_previous = semd_h;

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        if (cursor->s_semAdd == semAdd)
        {
            pcb_t* toReturn = removeProcQ(&(cursor->s_procQ));

            if (emptyProcQ(cursor->s_procQ))    // branch were the semaphore is emptied
            {                                   // it gets removed from the ASL
                cursor_previous->s_next = cursor->s_next;  

                semd_t* toAdd = cursor;     // we add the descriptor removed from the free descriptor list

                toAdd->s_next = semdFree_h;  // LIFO placement into semdFree_h
                semdFree_h = toAdd;
            }
            
            if (toReturn != NULL) toReturn->p_semAdd = NULL;
            return toReturn;
        }
        cursor_previous = cursor;   // cursor update
        cursor = cursor->s_next;
    }
    return NULL;
}



/*
    Removes the PCB pointed by p from the blocked semaphore's queue (p->p_semAdd).
    If a PCB isn't in the queue, returns NULL (error condition). Otherwise p is returned.
*/
pcb_t* outBlocked(pcb_t *p)
{
    semd_t* cursor = semd_h->s_next;
    semd_t* cursor_previous = semd_h;

    if (p == NULL || p->p_semAdd == NULL) return NULL;

    while (cursor->s_semAdd != (int*)MAXINT)
    {
        // if the PCB's semAdd is the one being looked for
        if(cursor->s_semAdd == p->p_semAdd)
        {
            pcb_t* toReturn = outProcQ(&(cursor->s_procQ), p);

            if (emptyProcQ(cursor->s_procQ))    // empty semaphore -> removed from ASL
            {
                cursor_previous->s_next = cursor->s_next;  

                semd_t* toAdd = cursor;     // we add the descriptor removed from the free descriptor list

                toAdd->s_next = semdFree_h;  // LIFO population into semdFree_h
                semdFree_h = toAdd;
            }

            if (toReturn != NULL) toReturn->p_semAdd = NULL;
            return toReturn;
        }
        cursor_previous = cursor;
        cursor = cursor->s_next;
    }
    return NULL;
}


/*
    Returns PCB's head pointer from the process queue associated to the SEMD with the semAdd key.
    Returns NULL if the SEMD isn't in the ASL or if it is but his process queue is empty. 
*/
pcb_t* headBlocked(int *semAdd)
{
    semd_t* hd = semd_h;

    while (hd->s_semAdd != (int*)MAXINT)
    {
        
        if(hd->s_semAdd == semAdd)
        {
            // checking if procQ is empty 
            if (hd->s_procQ == NULL) return NULL;
            // if it isn't it returns the heading pcb without removing it
            else return (hd->s_procQ)->p_next;

        }
        hd = hd->s_next;
    }
    return NULL;
}



/*
    Initializes the semdFree list so to fit all the semdTable elements.
    This method is invoked only once per data structure initialization.
    We use 2 dummy nodes to omptimazie the control over the list. 
*/
void initASL()
{
    semdFree_h = &semd_table[1];

    semd_t* hd = semdFree_h;

    int i = 2;

    while (i < MAXPROC + 1)
    {
        hd->s_next = &semd_table[i];
        hd = hd->s_next;
        i = i + 1;
    }

    hd->s_next = NULL;

    // dummy node at the start of the list
    semd_h = &semd_table[0];
    semd_h->s_semAdd = (int*)0x00000000;
    semd_h->s_procQ = NULL;

    // dummy node to the end of the list
    semd_h->s_next = &semd_table[MAXPROC +1];
    semd_h->s_next->s_semAdd = (int*)MAXINT;
    semd_h->s_next->s_procQ = NULL;

    semd_h->s_next->s_next = NULL;
}
