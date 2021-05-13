#include "../include/initProc.h"

/* U-proc state and support structs */
HIDDEN support_t U_support_structure[UPROCMAX];
HIDDEN state_t U_state_structure[UPROCMAX];

/* Swap pool table */
swap_t swap_table[POOLSIZE];

/* Swap pool devices semaphores*/
int swap_semaphores[UPROCMAX]; // 16 semaphores cause every process has 2 of them ? like in phase 2?
int mastersemaphore = 1;

void test()
{
    initSupportStructs();
    UProcInitiliazer();

    ///TODO: [initialize create ]
}

void UProcInitiliazer()
{


    for (int i = 0; i < UPROCMAX; i++)
    // e vaffanculo porcodio
        SYSCALL(1, (int) &U_state_structure[i], (int) &U_support_structure[i], 0);
}

void initSupportStructs()
{
    /* Set all semaphores to 1 cause of mutex */
    for(int i = 0; i < UPROCMAX; i++)
    {
        swap_semaphores[i] = 1;
    }

    /* Initialize u'proc processor state */
    int mask = 40;


    for(int i = 0; i < UPROCMAX; i++)
    {
        U_state_structure[i].pc_epc =  U_state_structure->reg_t9 =  (memaddr) 0x800000B0; 
        U_state_structure[i].reg_sp = (memaddr) 0xC0000000;
        U_state_structure[i].status = IMON | 0X00000003 | TEBITON ;
        U_state_structure[i].entry_hi =  mask; // not totally sure of this point

        mask = mask * 2; //we cycle the bits from the 7th to set the ASID
    }

    /* Initialize u'proc support structure */

    for(int i = 0; i < UPROCMAX; i++)
    {
        U_support_structure[i].sup_asid = i + 1 ; // are we sure about this ? oidocrop
        //TO-DO keep on initialize other fields declared at page 60 of pandos
    }
    

}