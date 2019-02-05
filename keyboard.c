#include "keyboard.h"

//#define KEYBOARD_DEBUG

#ifdef KEYBOARD_DEBUG
#include <stdio.h>
#endif

/**
 * @breif: key_state transfer
 * @param: sig: array for signal
 *              0----press of the key(W/A/S/D)
 *              1----in/not in high-speed-mode
 *              2----swing or not
 *              3----detecting whether G & C have same direction
 *         prevsig: array for the signal in the iteration before
 *         s: key-state
 *         x: current speed
 * @reval
*/
void kb_stateTransfer(const keysignal_t *key, const keysignal_t *high, bool_t *keyState, int *speed)
{
    if(keyState[0] == 0 && keyState[1] == 1)
    {
        //stop state
        if(key->n) //P
        {
            if(high->n) //P & H
            {
                //10: fast speeding up, the speed is initialized
                keyState[0] = 1;
                keyState[1] = 0;
                *speed += *speed == 0 ? INI_SPEED : 0;
            }
            else //P and ~H
            {
                if(*speed < LOW_SPEED) //speed is not as high as LOW_SPEED
                {
                    //11: slowly speeding up
                    keyState[0] = 1;
                    keyState[1] = 1;
                    *speed += *speed == 0 ? INI_SPEED : 0;
                }
                else if(*speed == LOW_SPEED) //speed meets the LOW_SPEED
                {
                    //00: holding the speed
                    keyState[0] = 0;
                    keyState[1] = 0;
                }
                //else: the speed is higher: stay in the stop mode so that the speed goed down
            }
        }
        //else (~P): stay in the stop mode to maintain the speed to be 0
    }
    else if(keyState[0] == 1 && keyState[1] == 1)
    {
        //Slowly speeding up
        if(!key->n) //~P
        {
            //01: stop state
            keyState[0] = 0;
            keyState[1] = 1;
        }
        else if(key->n && high->n) //P & H
        {
            //10: fast speeding up
            keyState[0] = 1;
            keyState[1] = 0;
        }
        else //P & ~H
        {
            if (*speed >= LOW_SPEED) //the speed is as high as LOW_SPEED
            {
                //00: holding the speed
                keyState[0] = keyState[1] = 0;
            }
            //else: remain in this state
        }
    }
    else if(keyState[0] == 1 && keyState[1] == 0)
    {
        //Fast speeding up
        if (!key->n || !high->n) //~P or ~H
        {
            //01: stop state to slow down
            keyState[0] = 0;
            keyState[1] = 1;
        }
        else //P and H
        {
            if (*speed >= HIGH_SPEED) //the speed is as high as the HIGH_SPEED
            {
                //00: holding the speed
                keyState[0] = 0;
                keyState[1] = 0;
            }
            //else: remain in the same state to speed up
        }
    }
    else
    {
        //s[0]==s[1]==0
        //holding speed
        if (key->n != key->p || high->n != high->p) //change: bakc to stop mode
        {
            keyState[0] = 0;
            keyState[1] = 1;
        }
        //else: remain
    }
    #ifdef KEYBOARD_DEBUG
        printf("INNER %d", *speed);
    #endif
}

/**
 * @breif: W/A/S/D control
 * @param: signal: array for signal
 *              0----press of the key(W/A/S/D)
 *              1----in/not in high-speed-mode
 *              2----swing or not
 *              3----detecting whether G & C have same direction
 *         prevSig: array for the signal in the iteration before
 *         keyState: array for key-state
 *         speed: current speed
 * @reval: the output speed
*/
int kb_keyControl(const keysignal_t *key, const keysignal_t *high, bool_t *keyState, int speed)
{
    bool_t keySig = (bool_t)key->n;
    bool_t keyPrev= (bool_t)key->p;
    bool_t highSig= (bool_t)high->n;
    bool_t highPrev=(bool_t)high->p;
    kb_stateTransfer(key,high, keyState, &speed);
    speed = ((1 + keyState[0] + keyState[0] + keyState[0]) * speed) >> keyState[1];
    speed = speed > HIGH_SPEED ? HIGH_SPEED : speed;
    return speed;
}

void kb_printState(char key, const bool_t *keyState)
{
    printf("\tKey %c: ", key);
    if (keyState[0] == 0 && keyState[1] == 1)
        printf("STOP              ");
    else if (keyState[0] == 0 && keyState[1] == 0)
        printf("HOLDING SPEED     ");
    else if (keyState[0] == 1 && keyState[1] == 1)
        printf("SLOWLY SPEEDING UP");
    else if (keyState[0] == 1 && keyState[1] == 0)
        printf("FAST SPEEDING UP  ");
    else
        printf("ERROR!");
}