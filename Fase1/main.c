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

/* memory allocation of the list by setting each list node to a memory address of the array
* up p_next: memory organization by setting a limit through memory addresses
*
*/
void initPcbs();
void initList(pcb_t* node);
void tail_insert(pcb_t* pcbFree_h, pcb_t* elem);
void head_insert(pcb_t* pcbFree_h,pcb_t* elem);

/*
void initPcbs()
{
    //setting up the cursor
    pcb_t* tmp = &pcbFree_h;
    p_sentinel->head = pcbFree_h;
    pcbFree_h = &pcbFree_table[0];
    pcbFree_h->val = 0;
    pcbFree_h = pcbFree_h->p_next;
    for (int i = 1; i<MAXPROC-1; i=i+1)
    {
        pcbFree_h = &pcbFree_table[i]; //malloc
        pcbFree_h->val = i;
        printf("val: %d, memaddr: %d\n",  pcbFree_h->val,  pcbFree_h);
        tmp->p_prev = &pcbFree_table[i-1];
        pcbFree_h = pcbFree_h->p_next;
    }
    p_sentinel->size = 0; //questo ci da problemi
    pcbFree_h = &pcbFree_table[MAXPROC];
    pcbFree_h->val = MAXPROC;
    tmp->p_prev = &pcbFree_table[MAXPROC-1];
    pcbFree_h->p_next = tmp;
    pcbFree_h = pcbFree_h->p_next;
    printList(pcbFree_h, MAXPROC);
    pcbFree_h->p_prev = &tmp;
    pcbFree_h = tmp;
}
*/

void initPcbs()
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

void tail_insert(pcb_t* list, pcb_t* elem)
{
    if(list != NULL)
    {
        if(list->p_next == NULL)
        {
            list->p_next == elem;
            list->p_prev == elem;
            elem->p_next = list;
            elem->p_prev = list;
        }
        else
        {
            pcb_t* tail = list->p_prev;
            tail->p_next = elem;
            elem->p_prev = tail;
            elem->p_next = list;
            list->p_prev = elem;
        }
    }

}

void head_insert(pcb_t* list,pcb_t* elem)
{
    if(list != NULL)
    {
        if(list->p_next == NULL)
        {
            list->p_next == elem;
            list->p_prev == elem;
            elem->p_next = list;
            elem->p_prev = list;
        }
        else
        {
            elem->p_next = list->p_next;
            elem->p_prev = list;
            list->p_next = elem;
            elem->p_next->p_prev = elem;
        }
    }
}

int emptyProcQ(pcb_t *tp)
{
    if (tp == NULL) return TRUE;
    else return FALSE;
}


void insertProcQ(pcb_t** tp, pcb_t* p)
{
    //printf("queue: %d p: %d\n", *tp, p);

    printf("\n\npasso 0\n");
    printf("p val:\n");
    printf("prev: %d | actual: %d | next: %d\n", p->p_prev, p, p->p_next);
    printf("tp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", (*tp)->p_prev, *tp , (*tp)->p_next);

    pcb_t* temp = (*tp)->p_prev;

    printf("\n\npasso 1\n");
    printf("temp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", temp->p_prev, temp , (temp)->p_next);
    printf("p val:\n");
    printf("prev: %d | actual: %d | next: %d\n", p->p_prev, p, p->p_next);
    printf("tp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", (*tp)->p_prev, *tp , (*tp)->p_next);

    (*tp)->p_prev = p;

    printf("\n\npasso 2\n");
    printf("temp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", temp->p_prev, temp , (temp)->p_next);
    printf("p val:\n");
    printf("prev: %d | actual: %d | next: %d\n", p->p_prev, p, p->p_next);
    printf("tp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", (*tp)->p_prev, *tp , (*tp)->p_next);

    p->p_prev = temp;

    printf("\n\npasso 3\n");
    printf("temp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", temp->p_prev, temp , (temp)->p_next);
    printf("p val:\n");
    printf("prev: %d | actual: %d | next: %d\n", p->p_prev, p, p->p_next);
    printf("tp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", (*tp)->p_prev, *tp , (*tp)->p_next);

    p->p_next = *tp;

    printf("\n\npasso 4\n");
    printf("temp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", temp->p_prev, temp , (temp)->p_next);
    printf("p val:\n");
    printf("prev: %d | actual: %d | next: %d\n", p->p_prev, p, p->p_next);
    printf("tp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", (*tp)->p_prev, *tp , (*tp)->p_next);

    (temp)->p_next = p;

    printf("\n\npasso 5\n");
    printf("temp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", temp->p_prev, temp , (temp)->p_next);
    printf("p val:\n");
    printf("prev: %d | actual: %d | next: %d\n", p->p_prev, p, p->p_next);
    printf("tp val:\n");
    printf("prev: %d | actual: %d | next: %d\n", (*tp)->p_prev, *tp , (*tp)->p_next);
}


void tail_insert_ini(pcb_t** tp, pcb_t* elem)
{
    if(*tp != NULL)
    {
        if((*tp)->p_next == NULL)
        {
            (*tp)->p_next == elem;
            (*tp)->p_prev == elem;
            elem->p_next = (*tp);
            elem->p_prev = (*tp);
        }
        else
        {
            pcb_t* tail = (*tp)->p_prev;
            tail->p_next = elem;
            elem->p_prev = tail;
            elem->p_next = (*tp);
            (*tp)->p_prev = elem;
        }
    }

}

int main()
{
    initPcbs();
    pcb_t* arrayTest;

    arrayTest = fillList(arrayTest, MemoryAlloc);

    //printf("\nListPrint");
   // printList(&pcbFree_h, MAXPROC+3);

    pcb_t tests;
    pcb_t* test;

    test = &tests;

    test->val = 69;

    printf("addr di pcbFree_h: %d addr di test: %d\n", pcbFree_h, test);

    //insertProcQ(&pcbFree_h, test);

    //insertProcQ2(&pcbFree_h, test);

    //printf("\nREVERSED\n", pcbFree_h, &pcbFree_h);
    //reversePrintList(&pcbFree_h, MAXPROC+3);

    //printList(pcbFree_h, MAXPROC+3);

    return 0;
}