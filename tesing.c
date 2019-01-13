#include <stdio.h>
#include "Control.h"
#include "testing.h"
#include "bool.h"


//global variables
int16_t             angle_inbet;  //Angle in between
rc_info_t           RcSig       = (rc_info_t)RcInit;
chassis_ctrl_t      ChassisSig  = (chassis_ctrl_t)chassisInit;
gimbal_ctrl_t       GimbalSig   = (gimbal_ctrl_t)gimbalInit;
ctrl_info_t         CtrlSig     = {.state={0,0,0,0},.gimbal_ctrl_ptr=&GimbalSig,.chassis_ctrl_ptr=&ChassisSig};

int main()
{
    int16_t buffer[18];
    int16_t feedback[2];//B and Error/exit
    //Part 0: get the signal
    getSignal(buffer);
    getFeedback(feedback);
    //In the test stage, the nessary signals comes from the input
    //there includes: [int] rc_f, rc_b, rc_left, rc_right, rc_upDown, rc_rotate, key, mouse_x, mouse_y
                    //[bool_t] W A S D H S B E
    while(feedback[1]!=1)
    {
        for(int i=0;i<18;i++)           printf("%4d ",buffer[i]);
        printf(" %4d %d",feedback[0],feedback[1]);
        rcDealler(buffer,feedback,&RcSig);
        refCalc(&RcSig, &CtrlSig);

        printState(&CtrlSig);
        printChassisRef(&CtrlSig);
        printGimbalRef(&CtrlSig);

        getSignal(buffer);
        getFeedback(feedback);
    }
    return 0;
}