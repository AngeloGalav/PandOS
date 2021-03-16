#include "asl.h"
#include "pcb.h"
#include "pandos_const.h"
#include "pandos_types.h"

typedef struct semd_t {
    /* ptr to next element on queue */
    struct semd_t *s_next;

    /* ptr to the semaphore */
    int *s_semAdd;  //(semaphore key)

    /* ptr to tail of the queue of procs. blocked on this sem. */
    pcb_PTR s_procQ;

} semd_t, *semd_PTR;