#ifndef SYSCALL_H_INCLUDED
#define SYSCALL_H_INCLUDED

#include "libraries.h"
#include "definitions.h"
#include "asl.h"
#include "pcb.h"

void SyscallExceptionHandler(state_t* exception_state);

/* Terminates a process */
HIDDEN void TerminateSingleProcess(pcb_t* to_terminate);

/* Kills a process tree recursively */
HIDDEN void TerminateTree(pcb_t* to_terminate);

/* CreateProcess (SYS1)*/
void Create_Process_SYS1();

void Terminate_Process_SYS2();

void Passeren_SYS3(int* semAddr);

void Verhogen_SYS4(int* semAddr);

void Wait_For_IO_Device_SYS5();

void Get_CPU_Time_SYS6();

void Wait_For_Clock_SYS7();

void Get_Support_Data_SYS8();

/* Additional measures adopted when returning from a syscall*/
void SyscallReturn();

/* Pass Up or Die applied to the process if the syscall requested is invalid */
void InvalidSyscall();

#endif