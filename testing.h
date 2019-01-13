#include "bool.h"
#include "Control.h"

void getSignal(int16_t *);//get signals from input and store them in the buffer pointed by uint8_t *
void getFeedback(int16_t *);
void printState(const ctrl_info_t *);
void printChassisRef(const ctrl_info_t *);
void printGimbalRef(const ctrl_info_t *);