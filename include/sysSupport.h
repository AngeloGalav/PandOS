#ifndef SYSSUPPORT_H_INCLUDED
#define SYSSUPPORT_H_INCLUDED

#include "pandos_const.h"
#include "definitions.h"

/* General exception handler for the Support level */
void GeneralException_Handler();

/* Terminates the current process , this is a wrapper for SYS2 */
void Terminate_SYS9(support_t* sPtr);

/* Reads the value of Time of Day Clock and put it inside reg_v0 register */
void Get_Tod_SYS10(support_t* sPtr);

/* Writes the string in a1 to the U-proc's printer device for the length of a2 */
void  Write_To_Printer_SYS11(support_t* sPtr);

/* Writes the string in a1 to the U-proc's terminal device for the length of a2 */
void Write_to_Terminal_SYS12(support_t* sPtr);

/* Reads the string written as an input of a terminal and places it in a buffer with address in a1 */
void Read_From_Terminal_SYS13(support_t* sPtr);

/* sets all entries in the swap table of a particular UPROC as empty */
void deleteSwTbEntries(int asid);

#endif