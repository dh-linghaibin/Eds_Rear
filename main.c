

#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"
#include "Eeprom.h"


int main( void ) {
    SysInit();
    EeepromInit();
    ControlInit();
    ComInit();
    LedInit();
    MoterInit();
    TimerInit();
    INTEN
    while(1) {
        if(TimerGetTimeFlag() == 60) {
            TimerClearTimeFlag();
            //MoterSleep();
            //ControlRunPosition(ControlCalculateGrating(i));
        }
        if(ComGetFlag() == 0x80) {
            u8 flag = 0;
            ComClearFlag();
            TimerClearTimeFlag();
            MoterOpen();
            LedSet(1);
            switch(ComGetData(0)) {
                case add_stal:
                    flag = ControlSetStallsAdd();
                    if(flag != 0x44) {
                        if(ControlRunPosition(ControlCalculateGrating(flag)) == 0x44) {
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        } else {
                            ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                        }
                    } else {
                        ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                    }
                break;
                case sub_stal:
                    flag = ControlSetStallsSub();
                    if(flag != 0x44) {
                        ControlRunPosition(ControlCalculateGrating(flag));
                    } else {
                        
                    }
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case add_setp:
                    //ControlRunPosition(-5);
                    MoterSpeed(1, 200);
                    DelayMs(30);
                    MoterSpeed(3,0);//stop
                break;
                case sub_setp:
                    //ControlRunPosition(5);
                    MoterSpeed(2, 200);
                    DelayMs(30);
                    MoterSpeed(3,0);//stop
                break;
                case dce_gear:
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case set_inti:
                    ControlSetStart();
                break;
                default:break;
            }
            ComClearData();
            LedSet(0);
            //MoterSleep();
        }
    }
}
