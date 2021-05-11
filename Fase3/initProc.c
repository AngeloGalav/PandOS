#include "../include/initProc.h"

/* U-proc state and support structs */
HIDDEN support_t U_support_structure[UPROCMAX];
state_t U_state_struture[UPROCMAX];


void test()
{
    initSupportStructs();

    ///TODO: [initialize create ]

}

void UProcInitiliazer()
{

    for (int i = 0; i < UPROCMAX; i++);
    //SYSCALL(1, (int) &U_state_struture[], (int) &U_support_structure, 0);
}

