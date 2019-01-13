#ifndef CONTROL_H
#define CONTROL_H

#include "bool.h"
#include "keyboard.h"

#define HIGH_SPEED        256
#define LOW_SPEED         128
#define INI_SPEED         1
#define ROTATION_SPEED    150
#define SWING_SPEED       90
#define SLOW_ROTATION     30
#define SPEED_CONST		  0.00068f
#define ANGLE_CONST       0.025f
#define RC_MIDDLE         1024u
#define ROTATION_CONST    0.01f
#define BACK_CONST        0.7f
#define ANGLE_ERROR       2

#define CW                    1
#define CCW                   -1

typedef   signed short     int int16_t;
typedef   signed          char int8_t;
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;

//Initlization definition
#define SIGINIT     {0,0}
#define KbStateInit {{False,True},{False,True},{False,True},{False,True}}
#define KbInit      {SIGINIT,SIGINIT,SIGINIT,SIGINIT,False,False,SIGINIT,False,KbStateInit,KbStateInit}
#define RcInit      {0,0,0,0,0,0,0,0,0,0,KbInit,0,0}
#define chassisInit {0,0,0}
#define gimbalInit  {0,0}
#define CtrlInit    {.state={False,False,False,False},.gimbal_ctrl_ptr=&GimbalSig,.chassis_ctrl_ptr=&ChassisSig}

typedef struct
{
    bool_t w_state[2];
    bool_t a_state[2];
    bool_t s_state[2];
    bool_t d_state[2];
} keyState_t;

typedef struct
{
    keysignal_t W;
    keysignal_t A;
    keysignal_t S;
    keysignal_t D;
    bool_t Q;
    bool_t E;
    keysignal_t High; //High speed
    bool_t Swing; //Swing
    keyState_t keyState; 
    keyState_t prevState; //useless, to be cleared
} KbCtrl_t;

typedef struct
{
    uint8_t state;
	/* rocker channel information */
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	int16_t ch4;
		
	/*keyboard-mouse information*/
	int16_t ch5;									//mouse-y
	int16_t ch6;									//mouse-x
	int16_t ch7;									//mouse-z
	uint8_t ch8;									//mouse-l
	uint8_t ch9;									//mouse-r
	KbCtrl_t kb_ctrl;

	/* left and right lever information */
	uint8_t sw1;
	uint8_t sw2;
} rc_info_t;

typedef struct
{
	int16_t forward_back_speed_ref;
	int16_t left_right_speed_ref;
	int16_t rotation_speed_ref;
} chassis_ctrl_t;

typedef struct
{
	int16_t horizontal_angle_ref;
	int16_t vertical_angle_ref;
} gimbal_ctrl_t;

typedef struct
{
    bool_t state[4]; //auto-maunal state & G-C state
    gimbal_ctrl_t * gimbal_ctrl_ptr;
    chassis_ctrl_t * chassis_ctrl_ptr;
} ctrl_info_t;

typedef struct
{
    uint8_t S : 1;
    uint8_t M : 1;
    uint8_t B : 1;
    uint8_t S1: 1;
    uint8_t S2: 1;
}OneBit;

void chassisGimbalInit(ctrl_info_t *);

void rcDealler(const int16_t *, const int16_t *, rc_info_t *); //STEP1: store the given signal into the rc_info_t struct
void refCalc(rc_info_t *, ctrl_info_t *); //STEP2: calculate the ref

void speed_calc(rc_info_t *, ctrl_info_t *); // calculate the f/b/l/f speed
                                             // not const since the keyState is going to be updated
void angle_cala(const rc_info_t *, ctrl_info_t *); // calculate the g_v_r, g_h_r and c_h_r
void state_transfer(const rc_info_t *, ctrl_info_t *); //update the state
void angle_determ(bool_t, ctrl_info_t *, int16_t, int16_t, int16_t); //determine the angles basing on the state

int16_t chassis_speed_key(keysignal_t *, keysignal_t *, bool_t *, int);
                                       // not const since the keyState is going to be updated
                                       //bool_t: the target signal, i.e. W/A/S/D
                                       //bool_t []: the current state of the target signal
int16_t chassis_angle_key(const KbCtrl_t *); 
void gimbal_angle_mouse(const rc_info_t *, gimbal_ctrl_t *);
// above is for key-mouse control and PC control
void chassis_speed_rc(const rc_info_t *, chassis_ctrl_t *);
void gimbal_angle_rc(const rc_info_t *, gimbal_ctrl_t *);
#endif