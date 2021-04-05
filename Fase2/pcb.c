#include "../include/pcb.h"

// Pointer to the unidirectional list of free and usable pcb_t's
HIDDEN pcb_t* pcbFree_h;

// pcb_t arrays with MAXPROC = 20 length, contains all concurrent processes.
HIDDEN pcb_t pcbFree_table[MAXPROC];



/*
    pcbFree_h list initialization, every element is added from the array process.
*/
void initPcbs()
{
    pcbFree_h = &pcbFree_table[0];
    pcb_t* hd = pcbFree_h;

    int i = 1;

    while (i < MAXPROC) // the list can't be greater than MAXPROC, indicating how many processes can fit.
    {
        hd->p_next = &pcbFree_table[i];
        hd = hd->p_next;
        i = i + 1;
    }

    hd->p_next = NULL;
}

/** Initializes every pcb's fields to NULL*/
void initializePcb(pcb_t* node)
{
    if (node != NULL)
    {
        node->p_next = NULL;
        node->p_prev = NULL;
        

        node->p_child = NULL;
        node->p_next_sib = NULL;
        node->p_prnt = NULL;
        node->p_prev_sib = NULL;
    }
}



/*
    Adds the PCB pointed by p into the free PCB list
*/
void freePcb(pcb_t* p)
{
    if (p != NULL)
    {
        p->p_prev = NULL;       // Faccio un inserimento in testa, mettendolo come primo elemento della lista.
        p->p_next = pcbFree_h;
        pcbFree_h = p;
    }
}

/** Restituisce NULL se la pcbFree_h è vuota.
*   Altrimenti rimuove un elemento dalla pcbFree, inizializza
*   tutti i campi (NULL/0) e restituisce l’elemento rimosso.
*/
/*
    Returns NULL in case the pcbFree_h is empty. Otherwise removes an element from pcbFree,
    calls the initialization and returns the removed element.
*/
pcb_t *allocPcb()
{
    if (pcbFree_h == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t* temp = pcbFree_h;
        pcbFree_h = pcbFree_h->p_next;  // copies a pcb_t from the pcbFree list.

        initializePcb(temp);           // it gets initialized and returned.

        return temp;
    }
}



/*
    creates a PCB list initialized to empty.
*/
pcb_t* mkEmptyProcQ()
{
    return NULL;
}



/*
    Returns TRUE if the pointed list is empty, FALSE otherwise.
*/
int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}



/*
    Adds the element pointed by p in the process queue pointed by tp.
    A double pointer is used for tp to add p as a last element.
*/
void insertProcQ(pcb_t** tp, pcb_t* p)
{
    if ((*tp) != NULL && p != NULL)
    {
        pcb_t* head = (*tp)->p_next;
        p->p_prev = (*tp);
        p->p_next = head; // The element is added

        head->p_prev = p;

        (*tp)->p_next = p;

        (*tp) = p;      // adding also a sentinel
    }
    else if ((*tp) == NULL && p != NULL)
    {
        (*tp) = p;
        (*tp)->p_next = (*tp);
        (*tp)->p_prev = (*tp);
    }
}



/*
    Returns the last element in the tp queue, returns NULL if there are no elements.
*/
pcb_t *headProcQ(pcb_t *tp)
{
    if (tp == NULL) return NULL;

    return tp->p_next;        // Returns the heading element.
}

/** Rimuove l’elemento piu’ vecchio dalla coda tp. Ritorna NULL se
*   la coda è vuota, altrimenti ritorna il puntatore all’elemento
*   rimosso dalla lista.
*/
/*
    Removes the oldest element in tp and returns its pointer.
    Returns NULL if the queue is empty.
*/
pcb_t* removeProcQ(pcb_t **tp)
{
    if (*tp == NULL) 
        return NULL;
    else if (*tp == (*tp)->p_next) // situation with a single element in the queue.
    {
        pcb_t* head = *tp;

        *tp = NULL; // Removes the only element.
        return head;
    }
    else    // branch for more elements in the queue.
    {
        pcb_t* head = (*tp)->p_next;    // Removes the heading element.
        (*tp)->p_next = head->p_next;

        pcb_t* tmp = head->p_next;
        tmp->p_prev = (*tp);

        return head;
    }
}

/** Rimuove il PCB puntato da p dalla coda dei processi puntata da tp. Se p non è presente
*   nella coda, restituisce NULL (p può trovarsi in una posizione arbitraria della coda).
*/
/*
    Removes the PCB pointed by p from the process queue pointed by tp.
    If p can't be found, NULL is returned.
*/
pcb_t* outProcQ(pcb_t **tp, pcb_t *p)
{
    if (tp == NULL || (*tp) == NULL || p == NULL) return NULL;
    else
    {
        if ((*tp) != p) // generic case where p isn't the first element
        {
            pcb_t* tmp = (*tp)->p_next; // we save in tmp the pointer to the rest of the list

            while (tmp != (*tp))
            {
                if (tmp == p)
                {
                    tmp->p_prev->p_next = tmp->p_next;
                    tmp->p_next->p_prev = tmp->p_prev;  // removes the element
                    return tmp;
                }
                tmp = tmp->p_next;
            }
            return NULL;
        }else if ((*tp) == (*tp)->p_next && (*tp) == p) // case in which p is the only element
        {
            pcb_t* tmp = (*tp);
            *tp = NULL;

            return tmp;
        }else                       // case in which the sentinel points to p but p isn't the only element.
        {
            pcb_t* tmp = (*tp);

            (*tp) = (*tp)->p_prev;

            (*tp)->p_next = tmp->p_next; // the rest of the list gets linked to the current element
            tmp->p_next->p_prev = (*tp); // indicate the new head

            return tmp;
        }
    }

    return NULL;
}



/*
    Returns TRUE if the pointed PCB has any children, FALSE otherwise
*/
int emptyChild(pcb_t *p)
{
    if ((p != NULL) && (p->p_child == NULL)) return TRUE;
    else return FALSE;
}



/*
    Adds the PCB pointed by p as a child to the PCB pointed by prnt  
*/
void insertChild(pcb_t *prnt, pcb_t *p)
{
    if (prnt != NULL && p != NULL)
    {
        pcb_t* tmp = prnt->p_child;     // LIFO population into the children list
        prnt->p_child = p;
        p->p_next_sib = tmp;

        p->p_prnt = prnt;

        if (tmp != NULL) tmp->p_prev_sib = p;    // if prnt had no children sets tmp->prev_sib to p,
                                                 // to handle bidirectionality
    }
}



/*
    Removes the first child from the pointed PCB, in case it has none NULL is returned.
*/
pcb_t* removeChild(pcb_t *p)
{
    if (p != NULL && p->p_child != NULL) // case of p with one child
    {
        pcb_t* tmp = p->p_child;         // tmp points to the first child

        if (tmp->p_next_sib != NULL)     // if p has rightmost siblings its not a lone child
        {
            p->p_child = tmp->p_next_sib;
            tmp->p_next_sib->p_prev_sib = NULL;
        } else
        {
            p->p_child = NULL;  // pointer to NULL for the last element
        }

        return tmp;
    }

    return NULL;
}

/** Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da
*   p non ha un padre, restituisce NULL, altrimenti restituisce l’elemento
*   rimosso (cioè p). A differenza della removeChild, p può trovarsi in una
*   posizione arbitraria (ossia non è necessariamente il primo figlio del padre).
*/
/*
    Removes the PCB pointed by p from the children list. If the PCB has no father NULL is returned,
    otherwise returns the removed element.
    This method can remove any p in the list, not only the first one.
*/
pcb_t *outChild(pcb_t* p)
{
    if (p == NULL) return NULL;
    if (p->p_prnt == NULL) return NULL; // creating two ifs in case of possible segfault

    if (p->p_prev_sib == NULL) return removeChild(p->p_prnt); // if p has no leftmost siblings, it's the first child
                                                              // and we can remove it with the previous method
    {
        pcb_t* tmp = p->p_prev_sib;
        tmp->p_next_sib = p->p_next_sib;

        if (p->p_next_sib != NULL)  // if there's a rightmost sibling we link it to the previous sibling
            p->p_next_sib->p_prev_sib = tmp;

        return p;
    }
}
