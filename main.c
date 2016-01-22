

#include "Sys.h"
#include "Com.h"
#include "Led.h"
#include "Moter.h"
#include "Timer.h"
#include <math.h>
#include "Delay.h"
#include "Control.h"

u8 i = 1;
u8 dr = 0;

int main( void ) {
    SysInit();
    ComInit();
    LedInit();
    MoterInit();
    TimerInit();
    INTEN
    while(1) {
        if(TimerGetTimeFlag() == 4) {
            TimerClearTimeFlag();
            ControlRunPosition(ControlCalculateGrating(i));
            if(dr == 0) {
                if(i < 10) {
                    i++;
                } else {
                    dr = 1;
                }
            } else {
                if(i > 0) {
                    i--;
                } else {
                    dr = 0;
                }
            }
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
