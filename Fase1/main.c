#include "pcb_test.h"

#define MAXPROC 20
#define HIDDEN static
#define TRUE            1
#define FALSE           0

//sentinella della lista pcbfree, punta a pcbfree_h
typedef struct sentinel{
    pcb_t* head;

    int size;
}sentinel;

sentinel *p_sentinel;

//puntatore alla lista dei pcb_t liberi e disponibili, non è un elemento della lista ,punta alla testa
pcb_t* pcbFree_h;

//array di pcb_t di lunghezza MAXPROC = 20
pcb_t pcbFree_table[MAXPROC];

//test
pcb_t MemoryAlloc[10];

/*  Ho fatto due funzioni initPcbs che inizializzano le due liste in modo di verso: con una, lo stesso puntatore pcbFree_h è nodo della lista,
 *  ciò significa che se volessi stampare l'intera lista mi tocca usare &pcbFree_h (che sarebbe l'indirizzo del primo nodo).
 *  Non consiglio di usare questa, ma la tengo per questioni legacy.
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

    pcb_t* allocTest;

    allocTest = allocPcb();

    insertProcQ(&pcbFree_h, test);

    printList(pcbFree_h, MAXPROC + 3);

    //printf("ELEMENTO TOLTO %d", allocTest);

    allocTest = mkEmptyProcQ();

    allocTest->val = 327;

    insertProcQ(&allocTest, test);
    printList(pcbFree_h, MAXPROC + 3);

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

//controlla se la lista è vuota
int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}

//l'elemento puntato da *p viene inserito nella lista di **tp (&pcbfree_h)
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
    pcb_t* list_head;

    initList(list_head);

    list_head->p_next = list_head;
    list_head->p_prev = list_head;

    return list_head;
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




