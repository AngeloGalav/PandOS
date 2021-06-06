#ifndef SYSSUPPORT_H_INCLUDED
#define SYSSUPPORT_H_INCLUDED

#include "pandos_const.h"
#include "definitions.h"


/* General exception handler for the Support level */
void GeneralException_Handler ();

/* The Support's level Syscall handler */
void Support_Syscall_Handler(support_t *sPtr);

/* Terminate the current process , this is a wrapper for SYS2 */
void Terminate_SYS9(support_t* sPtr);

/* Read the value of Time of Day Clock and put it inside reg_v0 register */
void Get_Tod_SYS10(unsigned int *regv0);

/* Write the string in a0 to the U-proc's printer device for the length of a2 */
void  Write_To_Printer_SYS11(support_t* sPtr);






#endif