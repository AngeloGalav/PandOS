#include "libraries.h"
#include "pandos_const.h"
#include "pandos_types.h"

/* Breakpoint function created for debugging the system */
void bp(void);

/* Extra breakpoint */
void bp_INIZIO(void);

/* Scheduler error breakpoint */
void bp_bhochestrano(void);

void bp_primoif();
void bp_p_inizio();
void bp_finisce_la_p();
void bp_wait();
void bp_interval();
void bp_godo();
void bp_syscall_(void);
void bp_correct();
void memcpy_start();
void memcpy_end();
void zombie_process_excp();
void bp_finitoIns();
void bp_entra_insBlock();
void bp_wait2();
void bp_ignore();
