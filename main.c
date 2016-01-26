

#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"
#include "Eeprom.h"


u8 i = 0;
u8 dr = 0;
u16 weizhi = 0;

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
        if(TimerGetTimeFlag() == 4) {
            TimerClearTimeFlag();
            if(dr == 0) {
                if(i < 10) {
                    i++;
                } else {
                    i = 9;
                    dr = 1;
                }
            } else {
                if(i > 0) {
                    i--;
                } else {
                    i = 1;
                    dr = 0;
                }
            }
            //ControlRunPosition(ControlCalculateGrating(i));
        }
        if(ComGetFlag() == 0x80) {
            ComClearFlag();
            switch(ComGetData(0)) {
                case add_stal:
                break;
                case sub_stal:
                break;
                case add_setp:
                break;
                case sub_setp:
                break;
                case dce_gear:
                break;
                case set_inti:
                break;
                default:break;
            }
        }
    }
}
