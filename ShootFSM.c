#include <stdio.h>
#include "ShootFSM.h"

//#define TEST

#define TIMEOVER_TEST 3
#define TIMEOVER      300
#define BURST_TIME	  3

int main(){
	  unsigned short int sig = 0;
	  unsigned int ctime = 0;
	  unsigned short int fireMode = 0;
	  unsigned short int fireInst = 0;
	  unsigned short int n = 0;
	  shoot_mode state = {0,0,0};
	  scanf("%d", &sig);
	  while(sig==0 || sig==1)
	  {
	  		printf("%d\t",sig);
				shootTask(&state, sig, &ctime);
				#ifdef TEST
	    	printShoot(&state);
	    	shoot_t fireCmd = shootCtrl(state.b,&fireMode,&fireInst,&n);
	    	printFire(&fireCmd);
				#else
				printf("PID: %d\n",setPIDRef(state.b,state.prev));
				state.prev = state.b;
				#endif
	    	scanf("%d", &sig);
	  }
	  return 0;
}

void shootTask(shoot_mode * mode, unsigned short int sig, unsigned int * ctime){
	  //conduct shoot state trans
	  OneBit_t onebit;
	  onebit.a    = mode->a;
	  onebit.b1   = mode->b/2;
	  onebit.b2   = mode->b%2;
	  onebit.Shoot= sig;
	  onebit.TimeO= *ctime>=TIMEOVER_TEST?1:0;

	  mode->a = (onebit.a & ~onebit.b1 & ~onebit.Shoot & ~onebit.TimeO)
	           | (~onebit.a & ~onebit.b1 & onebit.b2 & ~onebit.Shoot & ~onebit.TimeO);
	  mode->b = (onebit.b2 & onebit.Shoot & onebit.TimeO) | (onebit.b1 & onebit.b2 & onebit.Shoot)
	           | (onebit.a & onebit.Shoot & ~onebit.TimeO);
	  onebit.b2= (onebit.b2 & onebit.Shoot) |  (~onebit.a & ~onebit.b1 & onebit.Shoot);

	  mode->b = mode->b*2+onebit.b2;
	  *ctime = mode->b==3 || (mode->a==0 && mode->b==0)?0:*ctime+1;
	  printf("CTIME: %d\t",*ctime);
}
void printShoot(const shoot_mode * mode){
	  printf("MODE: %d%d%d\t", mode->a, mode->b/2, mode->b%2);
	  switch (mode->b)
	  {
	    case Nil:
	      printf("No Shooting\t");
	      break;
	    case Semi:
	      printf("Semi-Automatic shoot\t");
	      break;
	    case Auto:
	      printf("Automatic shoot\t");
	      break;
	    case Burst:
	      printf("Burst shoot\t");
	      break;
	    default:
	      printf("SHOOT MODE ERROR!\t");
	      break;
	  }
}

shoot_t shootCtrl(unsigned short int instru, unsigned short int * fireMode, unsigned short int * fireInst, unsigned short int * n){
	  //According to the shoot command, send order to the shoot mechanism
	  switch(*fireMode)
	  {
	  	case 0:
	  		*fireMode = instru;
	  		*fireInst = instru==0?0:1;
	  		break;
	  	case 1:
	  		if(*fireInst==1 || instru==1){
	  			*fireInst=0;
			}
			else if(instru==2 || instru==3){
				*fireMode=instru;
				*fireInst=1;
			}
			else{
				*fireMode = 0;
				*fireInst = 0;
			}
	  		break;
	  	case 2:
	  		if(*fireInst==1)
	  		{
	  			*fireMode = 2;
	  			*fireInst = 0;
			}
			else
			{
				if(*n>=BURST_TIME)
				{
					*fireMode = *fireInst==2?2:0;
					*fireInst = 0;
				}
				else
				{
					*fireMode = 2;
					*fireInst = 1;
				}
			}
	  		break;
	  	case 3:
	  		*fireMode = *fireInst==1 || instru==3?3:0;
	  		*fireInst = *fireInst==0 && instru==3?1:0;
	  		break;
	  	default:
	  		printf("FIRE COMMAND ERROR!\n");
	  		break;

	  }
	  if(*n>BURST_TIME)							*n=0;
	  if(*fireMode==2 && *fireInst==1)			*n+=1;

	  return (*fireInst?fire:wait);
}

void printFire(const shoot_t * cmd){
		if(*cmd==fire)
		{
			printf("FIRE!\n");
		}
		else
		{
			printf("WEAPON WAIT\n");
		}
}

int setPIDRef(unsigned short int cmd, unsigned short int prev_cmd)
{
	int rev;
	switch(cmd)
	{
		case 1:
			rev = prev_cmd==1?0:60;
			break;
		case 2:
			rev = prev_cmd==2?0:120;
			break;
		case 3:
			rev = prev_cmd==3?0:1200;
			break;
		default:
			rev = 0;
	}
	return rev;
}
