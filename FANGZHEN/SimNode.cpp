#include "SimNode.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "assert.h"

#include <string>
using namespace std;

#define uint  unsigned int
#define ulong unsigned long

SimNode::SimNode() {

	CURRENT_TIME    = 0;
	ALARM_TIME      = 0;

    TIMER_MODE      = 0;
    WORK_MODE       = 0;

    IP              = 0;

	CWmin           = 16;
	CWmax        	= 1024;
	CW           	= CWmin;
	CW_COUNTER      = 0;
	R_COUNTER = 0;

    SLOT            = 9;
	SIFS         	= 16;
	DIFS         	= 43;
    PHY_TIME        = 13.6;
	TRANS   	    = 0;
	MAC = 240;
	PAYLOAD = 1500 * 8;
	ACK = 32;
	ACKOUT = 65;


	RATE            = 455.8;

    PACKET_COUNTER  = 0;
    DROP_COUNTER    = 0;

    Random_Num      = 0;

    Prop_d          = 1;
    Prop_per        = 0;

    Halt_flag       = Not_Halt;

}

SimNode::~SimNode(){

}

void SimNode::inital(int ip,double Pd,double Per){
	IP       = ip;
	Prop_d   = Pd;
	Prop_per = Per;       
}

void SimNode::run(int clock_in,int flag_channel){

	CURRENT_TIME = clock_in;

	switch(TIMER_MODE){
		case DOWN_TIMER:
		{
			switch(WORK_MODE){
				case IDLE_STATE:
					assert(CW_COUNTER <= 0);
					if (R_COUNTER >= 32) {   //超过重传次数，重新传送
						CW = CWmin;
						R_COUNTER =0;
					}
					CW_COUNTER = ulrand() % CW;
					set_alarm(DIFS);
					WORK_MODE = BACKOFF_STATE;
					break;
				case BACKOFF_STATE:
					if(CW_COUNTER <= 0){
						WORK_MODE = SEND_STATE;
					}
					else if(CW_COUNTER > 0){
						if(Halt_flag == Halt){
							set_alarm(1);
							Halt_flag = Not_Halt;
						}
						else if(Halt_flag == Not_Halt){
							CW_COUNTER = CW_COUNTER - 1;
							set_alarm(SLOT);
						}
					}
					break;
				case SEND_STATE:
					TRANS = DIFS + PHY_TIME + (MAC + PAYLOAD) / RATE + ACK + SIFS;
					set_alarm(TRANS);
					WORK_MODE = RECEIVE_STATE;
					break;
				case RECEIVE_STATE:
                                Random_Num = (double)ulrand()/ulRAND_MAX;
								PACKET_COUNTER = PACKET_COUNTER + 1;
                                if( Random_Num < ( Prop_d*1 + (1-Prop_d)*Prop_per ) )
                                {
                                    //PACKET_COUNTER = PACKET_COUNTER + 1;
				       }					
					CW             = CWmin;
					WORK_MODE      = IDLE_STATE;
					break;
				case COLLISION_STATE:
					TRANS = DIFS + PHY_TIME + ACKOUT+ (MAC + PAYLOAD) / RATE;
					set_alarm(TRANS);
					R_COUNTER++;
					WORK_MODE = ACKTIMEOUT_STATE;
					break;
				case ACKTIMEOUT_STATE:
					WORK_MODE = IDLE_STATE;
					break;
			}
			break;
		}
		case UP_TIMER:
		{
			this->timer(flag_channel);
			break;
		}
	}
}

unsigned long SimNode::ulrand(void) {
    return (
     (((unsigned long)rand()<<24)&0xFF000000ul)
    |(((unsigned long)rand()<<12)&0x00FFF000ul)
    |(((unsigned long)rand()    )&0x00000FFFul));
}

void SimNode::timer(int flag_channel){
	if(WORK_MODE==BACKOFF_STATE || WORK_MODE==IDLE_STATE)
		if(flag_channel==BUSY){
			ALARM_TIME = 1 + CURRENT_TIME;
			Halt_flag = Halt;
		}
	if(CURRENT_TIME >= ALARM_TIME){
		TIMER_MODE = DOWN_TIMER;
	}
}

void SimNode::set_alarm(ulong alarm_time){
	ALARM_TIME = alarm_time + CURRENT_TIME;
	TIMER_MODE = UP_TIMER;
}
