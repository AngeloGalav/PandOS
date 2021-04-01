#include "libraries.h"
#include "pandos_const.h"
#include "pandos_types.h"

/* Breakpoint function created for debugging the system */
void bp(void);

/* Extra breakpoint */
void bp_extra(void);

/* Scheduler error breakpoint */
void bp_scheduler_error(void);

void bp_adel();
void bp_process_dispatched();
void bp_sys_7();
void bp_wait();
void bp_interval();
void bp_device();
void bp_syscall_(void);
void bp_correct();
void memcpy_start();
void memcpy_end();
void zombie_process_excp();
void bp_terminal_correct();
void bp_ALERT();
void bp_wait2();

