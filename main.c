<<<<<<< HEAD

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
    //TimerInit();
    DelayMs(50);//等待系统上电稳定
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
            u16 data;
            ComClearFlag();
            TimerClearTimeFlag();
            MoterOpen();
            LedSetModeFlicker(1);
            switch(ComGetData(0)) {
                case add_stal:
                    flag = ControlSetStallsAdd();
                    if(flag != 0x44) {
                        if(ControlRunPosition(ControlCalculateGrating(flag)) == 0x44) {
                            DelayMs(20);//等待系统上电稳定
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
                            DelayMs(20);//等待系统上电稳定
                            ComSendCmd(stuck, ControlGetStall() ,0 ,0);
                        } else {
                            ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                        }
                    } else {
                        ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                    }
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                break;
                case add_setp://增加位置保护
                    adr = ConterResistancePositionFiltering();//get position
                    if(adr > 100) {
                        ControlSetpSmall(3,1);
                    }
                break;
                case sub_setp://增加位置保护
                    adr = ConterResistancePositionFiltering();//get position
                    if(adr < 60000) {
                        ControlSetpSmall(3,2);
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
                            DelayMs(20);//等待系统上电稳定
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
                case 0x10://询问当前档位
                    ComSendCmd(dce_gear, ControlGetStall() ,0 ,0);
                    break;
                case 0x11://设置档位值
                    data = TypeCombinationU16(ComGetData(2),ComGetData(3));
                    ControlSetStalls(ComGetData(1),data);
                    break;
                case 0x12://设置初始
                    data = TypeCombinationU16(ComGetData(1),ComGetData(2));
                    ControlSetStallsStart(data);
                    break;
                case 0x13:
                    data = TypeCombinationU16(ComGetData(1),ComGetData(2));
                    ControlEleSet(data);
                    break;
                case 0x14:
                    break;
                case 0x15:
                    break;
                default:break;
            }
            ComClearData();
            //LedSet(0);
        }
    }
}
=======

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
    DelayMs(50);//等待系统上电稳定
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
                            DelayMs(20);//等待系统上电稳定
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
                            DelayMs(20);//等待系统上电稳定
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
                case add_setp://增加位置保护
                    adr = ConterResistancePositionFiltering();//get position
                    if(adr > 100) {
                        ControlSetp(5,1);
                    }
                break;
                case sub_setp://增加位置保护
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
                            DelayMs(20);//等待系统上电稳定
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
>>>>>>> origin/master
