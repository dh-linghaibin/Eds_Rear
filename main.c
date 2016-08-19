
#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"
#include "Eeprom.h"

u16 xxx = 0;

int main( void ) {
    SysInit();
    EeepromInit();
    ControlInit();
    ComInit();
    LedInit();
    MoterInit();
    TimerInit();
    DelayMs(50);//�ȴ�ϵͳ�ϵ��ȶ�
    INTEN
    while(1) {
        //xxx = ConterResistancePositionFiltering();//get position
        if(TimerGetTimeFlag() == 60) {
            TimerClearTimeFlag();
           // MoterSleep();
        }
        LedTimeService();
        if(ComGetFlag() == 0x80) {
            u8 flag = 0;
            u16 adr = 0;
            ComClearFlag();
            TimerClearTimeFlag();
            MoterOpen();
            LedSetModeFlicker(1);
            switch(ComGetData(0)) {
                case add_stal:
                    flag = ControlSetStallsAdd();
                    if(flag != 0x44) {
                        if(ControlRunPosition(ControlCalculateGrating(flag)) == 0x44) {
                            DelayMs(20);//�ȴ�ϵͳ�ϵ��ȶ�
                            //ControlSetStallsSub();
                            //ControlRunPosition(ControlCalculateGrating(flag));
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
                        if( ControlRunPosition(ControlCalculateGrating(flag)) == 0x44) {
                            DelayMs(20);//�ȴ�ϵͳ�ϵ��ȶ�
                           // ControlSetStallsAdd();
                           // ControlRunPosition(ControlCalculateGrating(flag));
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        } else {
                            ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                        }
                    } else {
                        ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                    }
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case add_setp://����λ�ñ���
                    adr = ConterResistancePositionFiltering();//get position
                    if(adr > 100) {
                        ControlSetp(5,1);
                    }
                break;
                case sub_setp://����λ�ñ���
                    adr = ConterResistancePositionFiltering();//get position
                    if(adr < 60000) {
                        ControlSetp(5,2);
                    }
                break;
                case dce_gear:
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case set_inti:
                    ControlSetStart();
                break;
                case set_gear:
                    flag = ControlSetStallsSet(ComGetData(1));
                    if(flag != 0x44) {
                        if(ControlRunPosition(ControlCalculateGrating(flag)) == 0x44) {
                            DelayMs(20);//�ȴ�ϵͳ�ϵ��ȶ�
                            //ControlSetStallsSub();
                            //ControlRunPosition(ControlCalculateGrating(flag));
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        } else {
                            ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                        }
                    } else {
                        ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                    }
                break;
                default:break;
            }
            ComClearData();
            //LedSet(0);
        }
    }
}
