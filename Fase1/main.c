#include "pcb_test.h"

#define MAXPROC 20
#define HIDDEN static
#define TRUE            1
#define FALSE           0

/*
typedef struct pcbFree_h{
    pcb_t *elem;
    struct pcbFree_h *p_next,*p_prev;
}pcbFree;
*/

typedef struct sentinel{
    pcb_t* head;

    int size;
}sentinel;

sentinel *p_sentinel;
pcb_t* pcbFree_h;
pcb_t pcbFree_table[MAXPROC];

pcb_t MemoryAlloc[10];

/*  Ho fatto due funzioni initPcbs che inizializzano le due liste in modo di verso: con una, lo stesso puntatore pcbFree_h è nodo della lista,
 *  ciò significa che se volessi stampare l'intera lista mi tocca usare &pcbFree_h (che sarebbe l'indirizzo del primo nodo).
 *  Non consiglio di usare questa, ma la tengo per questioni legacy. (E per far capire a quello scemo di Adriano come funzionano i puntatori (ಠ_ಠ) )
*/
void initPcbs_2();

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



int main()
{
    initPcbs();
    pcb_t* arrayTest;

    //this is a test array, ignore it
    arrayTest = fillList(arrayTest, MemoryAlloc);

    //pcb_t making
    pcb_t tests;
    pcb_t* test;
    test = &tests;
    test->val = 69;

    insertProcQ(&pcbFree_h, test);

    printList(pcbFree_h, MAXPROC + 3);

    return 0;
}

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
}

void initPcbs_2()
{
    pcb_t* hd = &pcbFree_h;

    hd->val = 0;
    hd->p_prev = &pcbFree_table[MAXPROC-2];
    hd->p_next = &pcbFree_table[0];
    hd = hd->p_next;


    hd->val = 1;
    hd->p_prev = &pcbFree_h;
    hd->p_next = &pcbFree_table[1];
    hd = hd->p_next;

    int i = 2;

    while (i < MAXPROC - 1){
        hd->val = i;
        hd->p_prev = &pcbFree_table[i-2];
        hd->p_next = &pcbFree_table[i];
        i = i + 1;
        hd = hd->p_next;
    }

    hd->val = i;
    hd->p_prev = &pcbFree_table[i-2];
    hd->p_next = &pcbFree_h;
}

int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}

void insertProcQ(pcb_t** tp, pcb_t* p)
{
    pcb_t* hd = (*tp)->p_prev;

    p->p_prev = (*tp)->p_prev;
    p->p_next = (*tp);

    hd->p_next = p;

    (*tp)->p_prev = p;
}

pcb_t *allocPcb()
{
    if (pcbFree_h == NULL)
    {
        return NULL;
    }
    else
    {
        pcb_t* temp = pcbFree_h;
        pcb_t* tail = pcbFree_h->p_prev;
        p_sentinel->head = pcbFree_h->p_next;

        initList(temp);

        tail->p_next = p_sentinel->head;
        p_sentinel->head = NULL;

        return temp;
    }
}

void freePcb(pcb_t* p)
{
    if (p != NULL && pcbFree_h != NULL)
    {
        tail_insert(pcbFree_h, p);
    }
}

pcb_t* mkEmptyProcQ()
{
    pcb_t* list_head;

    initList(list_head);

    list_head->p_next = list_head;
    list_head->p_prev = list_head;

    return list_head;
}

void initList(pcb_t* node)
{
    node->p_next = NULL;
    node->p_prev = NULL;
    node->p_child = NULL;
    node->p_next_sib = NULL;
    node->p_prnt = NULL;
    //node->p_s = NULL;
    node->p_prev_sib = NULL;
}




