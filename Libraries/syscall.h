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

pcb_t* Verhogen_SYS4(int* semAddr);

void Wait_For_IO_Device_SYS5();

void Get_CPU_Time_SYS6();

void Wait_For_Clock_SYS7();

void Get_Support_Data_SYS8();

/* Additional measures adopted when returning from a syscall*/
void SYSCALL_Return();

#endif