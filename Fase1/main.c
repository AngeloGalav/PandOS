#include "pcb_test.h"

#define MAXPROC 20
#define HIDDEN static
#define TRUE            1
#define FALSE           0

//sentinella della lista pcbfree, punta a pcbfree_h
typedef struct sentinel{
    pcb_t* head;

}sentinel;

sentinel *p_sentinel;

//puntatore alla lista dei pcb_t liberi e disponibili, non è un elemento della lista ,punta alla testa
pcb_t* pcbFree_h;

//array di pcb_t di lunghezza MAXPROC = 20
pcb_t pcbFree_table[MAXPROC];

//memoria dei processi (usata per testare)
pcb_t pcb_memory[300];

/*  Questa è la initPcbs che funziona come si deve, ovvere pcbFree punta al primo elemento della lista e basta, senza essere anch'esso il primo
 *  elemento. Dunque, per stampare la lista intera ti basta usare pcbFree_h (e anche per lavorare su pcbFree_h ti basta usare pcbFree_h senza
 *  la &). Questa è la funzione che consiglio di usare.
*/

void initPcbs();

void initList(pcb_t* node);

void freePcb(pcb_t* p);

pcb_t *allocPcb();

pcb_t* mkEmptyProcQ();

int emptyProcQ(pcb_t *tp);

void insertProcQ(pcb_t** tp, pcb_t* p);

pcb_t *headProcQ(pcb_t **tp);

pcb_t* removeProcQ(pcb_t **tp);

pcb_t* outProcQ(pcb_t **tp, pcb_t *p);

///FUNZIONI SUGLI ALBERI///

int emptyChild(pcb_t *p);

void insertChild(pcb_t *prnt, pcb_t *p);

pcb_t* removeChild(pcb_t *p);

pcb_t *outChild(pcb_t* p);


int i = 0;

int main()
{
    initPcbs();
    /*
    //pcb_t making
    pcb_t tests;
    pcb_t test2;
    pcb_t* test;
    test = &tests;
    test->val = 69;

    pcb_t* allocTest;
    pcb_t* allocTest1;
    pcb_t* allocTest2;

    allocTest1 = &test2;
    allocTest1->val = 909;

    insertProcQ(&pcbFree_h, test);
    insertProcQ(&pcbFree_h, allocTest1);

    printList(pcbFree_h, MAXPROC + 3);

    pcb_t* work = NULL;

    work = outProcQ(&pcbFree_h, test);

    printList(pcbFree_h, MAXPROC + 3);
    printf("ELEMENTO ELIMINATO: %d | addr: %d\n", work->val, work);

    */

    pcb_t* tree_source;
    tree_source = fillTree_UGLY(tree_source, pcb_memory, 5);

    printPcbTree(tree_source);

    //pcbFree_h->val = 69;

    //insertChild((tree_source->p_child)->p_next_sib, pcbFree_h);

    //printPcbTree(tree_source);

    pcbFree_h = removeChild(tree_source->p_child);

    //pcbFree_h = outChild((tree_source->p_child)->p_next_sib);

    if (pcbFree_h == NULL)
    {
        printf("CANNOT DELETE NULL ELEMENT!\n");

    }else{

        printf("DELETED ELEMENT IS %d, value of %d\n" , pcbFree_h, pcbFree_h->val);

        if (pcbFree_h->p_prnt == NULL && pcbFree_h->p_prev_sib == NULL && pcbFree_h->p_next_sib == NULL)
        {
            printf("COMPLETELY DELETED CLEAN\n");
        }
    }

    printPcbTree(tree_source);

    return 0;
}

//inizializza la lista di pcbfree_h con i pcb_t che sono nell'array pcbfree_table
void initPcbs()
{
    pcbFree_h = &pcbFree_table[0];
    pcb_t* hd = pcbFree_h;

    hd->val = 0;
    hd->p_prev = &pcbFree_table[MAXPROC - 1];
    hd->p_next = &pcbFree_table[1];

    hd = hd->p_next;

    int i = 1;

    while (i < MAXPROC - 1){
        hd->val = i;
        hd->p_prev = &pcbFree_table[i-1];
        hd->p_next = &pcbFree_table[i+1];

        i = i + 1;
        hd = hd->p_next;
    }

    hd->val = i;
    hd->p_prev = &pcbFree_table[i - 1];
    hd->p_next = &pcbFree_table[0];

    p_sentinel->head = pcbFree_h;
}

//controlla se la lista è vuota
int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}

//l'elemento puntato da *p viene inserito nella lista di **tp (&pcbfree_h)
/**
 * inserisce l’elemento puntato da p nella
 * coda dei processi tp. La doppia
 * indirezione su tp serve per poter inserire
 * p come ultimo elemento della coda.
 */
void insertProcQ(pcb_t** tp, pcb_t* p)
{
    pcb_t* hd = (*tp)->p_prev;

    p->p_prev = (*tp)->p_prev;
    p->p_next = (*tp);

    hd->p_next = p;

    (*tp)->p_prev = p;
}

//se la lista di pcb_t è vuota ritorna null altrimenti  e ritorna un elemento dopo averlo rimosso
pcb_t *allocPcb() //TODO: AGGIUNGERE SUPPORTO SENTINELLA
{
    if (pcbFree_h == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t* temp = pcbFree_h;

        pcb_t* tail = pcbFree_h->p_prev;
        pcb_t* nx = pcbFree_h->p_next;

        //p_sentinel->head = pcbFree_h->p_next;

        nx->p_prev = tail;
        tail->p_next = nx;

        pcbFree_h = nx;

        initList(temp);

        //tail->p_next = p_sentinel->head;
        //p_sentinel->head = NULL;

        return temp;
    }
}

// inserisce(in coda ) il pcb_t puntato da *p nella lista pcbfree_h
void freePcb(pcb_t* p)
{
    if (p != NULL && pcbFree_h != NULL)
    {
        tail_insert(pcbFree_h, p);
    }
}

// crea una lista di pcb_t inizializzandola come vuota
pcb_t* mkEmptyProcQ()
{

    pcb_t* list_head = NULL; //&pcb_memory[0 + i];
    /*
    //initList(list_head);

    list_head->val = 78 + i;

    list_head->p_next = list_head;
    list_head->p_prev = list_head;

    i = i + 1;
    */
    return list_head;
}

/**
 * Restituisce l’elemento in fondo alla coda dei processi tp, SENZA RIMUOVERLO.
 * Ritorna NULL se la coda non ha elementi.
 */
pcb_t *headProcQ(pcb_t **tp)
{
    if(*tp == NULL)
    {
        return NULL;
    }
    else
    {
        return (*tp)->p_prev;
    }
}

/**
 * Rimuove l’elemento piu’ vecchio dalla coda
 * tp. Ritorna NULL se la coda è vuota,
 * altrimenti ritorna il puntatore all’elemento
 * rimosso dalla lista.
 */
pcb_t* removeProcQ(pcb_t **tp)
{
    if(*tp == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t *tmp = *tp;
        *tp = (*tp)->p_next;
        (*tp)->p_prev = tmp->p_prev;
        tmp->p_prev->p_next = (*tp);
        return tmp;
    }
}

/** Rimuove il PCB puntato da p dalla coda dei
*   processi puntata da tp. Se p non è presente
*   nella coda, restituisce NULL (p può trovarsi
*   in una posizione arbitraria della coda).
*/
pcb_t* outProcQ(pcb_t **tp, pcb_t *p)
{
    if( (tp != NULL) && (*tp != NULL) && (p != NULL)  && ((*tp) != p)) // p non è il primo elemento
    {
        pcb_t* tmp = (*tp)->p_next;
        while(tmp != (*tp) )
        {
            if(tmp == p)
            {
                tmp->p_prev->p_next = tmp->p_next;
                tmp->p_next->p_prev = tmp->p_prev;
                tmp->p_next = NULL;
                tmp->p_prev = NULL;
                return tmp;
            }
            tmp = tmp->p_next;
        }
        return NULL;
    }
    else if ( (*tp) == p &&(tp != NULL) &&(*tp) != NULL) // p è il primo elemento
    {
            pcb_t* tmp = (*tp);
            (*tp) = (*tp)->p_next;
            (*tp)->p_prev = tmp->p_prev;
            tmp->p_prev->p_next = (*tp);
            return tmp;
    }
    else
    {
        return NULL;
    }

}

///FUNZIONI ALBERI///

//TODO: MAKE PRINT, FILL functions on trees in pcb_test.h

/** Restituisce TRUE se il PCB puntato da p
*   non ha figli, FALSE altrimenti.
*/
int emptyChild(pcb_t *p)    //restituisce true se è una foglia!
{
    if ( p != NULL && p->p_child == NULL)
    {
        return TRUE;
    } else {
        return FALSE;
    }
}

/** Inserisce il PCB puntato da p come figlio
*   del PCB puntato da prnt.
*/
void insertChild(pcb_t *prnt, pcb_t *p) //inserisce un figlio p a prnt.
{
    if (prnt != NULL && p != NULL)  //se il figlio non esiste, evito il segFault.
    {
        pcb_t* tmp = prnt->p_child;
        prnt->p_child = p;
        p->p_next_sib = tmp;

        p->p_prnt = prnt;

        if (tmp != NULL){
            tmp->p_prev_sib = p;    //se prnt non aveva figli inizialmente, mi basta mettere tmp->prev_sib a p;
        }
    }

}

/** Rimuove il primo figlio del PCB puntato
*   da p. Se p non ha figli, restituisce NULL.
*/
pcb_t* removeChild(pcb_t *p)
{
    if (p != NULL && p->p_child != NULL) //se p non ha un figlio, ritorno NULL
    {
        pcb_t* tmp = p->p_child;    //tmp è il figlio

        if (tmp->p_next_sib != NULL){ //se p non ha fratelli destri, evito il segFault
            p->p_child = tmp->p_next_sib;
            tmp->p_next_sib->p_prev_sib = NULL;
        } else {
            p->p_child = NULL;  //se p aveva un solo figlio, mi basta mettere p->child = null;
        }

        /*  Ho aggiunto queste due righe nel caso il prof volesse che quando potassimo l'albero non ci sia più traccia dell'albero
        *   d'origine. Ma ciò dipende tutto da cosa vuole lui. Nel dubbio, queste due righe si possono sempre togliere.
        */
        tmp->p_prnt = NULL;
        tmp->p_next_sib = NULL;

        return tmp;
    }

    return NULL;
}

/** Rimuove il PCB puntato da p dalla lista
*   dei figli del padre. Se il PCB puntato da
*   p non ha un padre, restituisce NULL,
*   altrimenti restituisce l’elemento
*   rimosso (cioè p). A differenza della
*   removeChild, p può trovarsi in una
*   posizione arbitraria (ossia non è
*   necessariamente il primo figlio del padre).
*/
pcb_t *outChild(pcb_t* p)   //ho interpretato questa funzione nel caso si intendesse che p sia un elemento della lista dei figlio di p->prnt
{

    if (p->p_prnt == NULL)
    {
        return NULL;
    }


    if (p->p_prev_sib == NULL)
    {
        return removeChild(p->p_prnt); //se non ha un fratello sinistro, p è sicuramente il primo figlio. posso quindi usare la funzione vecchia
    } else
    {
        pcb_t* tmp = p->p_prev_sib; //se è un elemento in mezzo o alla fine, mi basta eseguire queste due righe.
        tmp->p_next_sib = p->p_next_sib;

        if (p->p_next_sib != NULL) //se non ha un fratello destro, evito il segFault;
        {
            p->p_next_sib->p_prev_sib = tmp;
        }

        /*  Ho aggiunto queste tre righe nel caso il prof volesse che quando potassimo l'albero non ci sia più traccia dell'albero
        *   d'origine. Ma ciò dipende tutto da cosa vuole lui. Nel dubbio, queste tre righe si possono sempre togliere.
        */
        p->p_prnt = NULL;
        p->p_next_sib = NULL;
        p->p_prev_sib = NULL;

        return p;
    }
}


//funzione ausiliaria per inizializzare i campi a NULL
void initList(pcb_t* node)
{
    if (node != NULL){
        node->p_next = NULL;
        node->p_prev = NULL;
        node->p_child = NULL;
        node->p_next_sib = NULL;
        //node->p_s = 0;
        node->p_prnt = NULL;
        node->p_prev_sib = NULL;
    }
}




