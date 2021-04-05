#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "asl.h"
#include "pcb.h"
#include "scheduler.h"

/*  Handles syscall exceptions by executing the syscall function associated with the syscall code 
 *  in the a0 register of the exception_state param.
 */
void SyscallExceptionHandler(state_t* exception_state);

/* Creates a process and sets it as the child of the current process */
void Create_Process_SYS1();

/* Terminates the current process and all his children */
void Terminate_Process_SYS2();

/* Terminates a given process */
HIDDEN void TerminateSingleProcess(pcb_t* to_terminate);

/* Kills a process tree recursively */
HIDDEN void TerminateTree(pcb_t* to_terminate);

/* Executes a P operation on a given semaphore (this syscall is reserved for non-device semaphores) */
void Passeren_SYS3(int* semAddr);

/* Executes a V operation on a given semaphore (this syscall is reserved for non-device semaphores) */
void Verhogen_SYS4(int* semAddr);

/* Executes a P operation on a device semaphore */
void Wait_For_IO_Device_SYS5();

/* Returns the total CPU time used by the current process */
void Get_CPU_Time_SYS6();

/* Executes a P operation on the pseudo-clock semaphore */
void Wait_For_Clock_SYS7();

/* Returns the support level information of the current process */
void Get_Support_Data_SYS8();

/* Applies a Pass Up or Die to the process if the syscall requested is invalid (i.e. with syscall code above 8) */
void InvalidSyscall();

#endif