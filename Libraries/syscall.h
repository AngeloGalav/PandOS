#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "asl.h"
#include "pcb.h"

void SyscallExceptionHandler(state_t* exception_state);

/* Terminates a process */
HIDDEN void TerminateSingleProcess(pcb_t* to_terminate);

/* Kills a process tree (and siblings) recursively */
HIDDEN void KillRec(pcb_PTR proc_elem);

/* CreateProcess (SYS1)*/
void Create_Process_SYS1(state_t arg1, support_t* arg2);

void Terminate_Process_SYS2();

void Passeren_SYS3(int* semAddr);

void Verhogen_SYS4(int* semAddr);

void Wait_For_IO_Device_SYS5(int intlNo, int dnum, int waitForTermRead);

void Get_CPU_Time_SYS6();

void Wait_For_Clock_SYS7();

void Get_Support_Data_SYS8();

/* Additional measures adopted in syscall that can block processes (i.e. SYS3, SYS5 and SYS7) */
HIDDEN void BlockingSyscallAdjustments();

#endif