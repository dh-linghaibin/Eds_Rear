
#include "Control.h"
#include "Moter.h"
#include "Timer.h"

#define TRANDFORMATIONP 50

#define SPEEDP 2.5

static u16 stalls_add[11] = {0,2000,4000,6000,8000,10000,12000,14000,16000,18000,20000};

void ControlInit(void) {

}

static int num = 0;

int ControlCalculateGrating(u8 stalss) {
    u16 res_position_absolutely = 0;
    u16 res_position_new = 0;
    u16 res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
    res_position_absolutely = stalls_add[stalss] + 19000;//count position
    res_position_new = MoterReadResistancePosition();//get position
    if(res_position_absolutely > res_position_new) {
        symbol_bit = 0;
        res_position_different = res_position_absolutely - res_position_new;
    } else {
        symbol_bit = 1;
        res_position_different = res_position_new - res_position_absolutely;
    }
    rotate_num = (u16)(res_position_different/TRANDFORMATIONP);
    if(symbol_bit == 1) {
        rotate_num = 0 - rotate_num;
    }
    num = rotate_num;
    return rotate_num;
}

u16 current = 0;

u8 ControlRunPosition(int num) {
    static u8 dr = 0;
    static u16 position_difference = 0;
    static u8 sleep_sub = 0;
    MoterSetCodingSite(0);//clear
    if(num > 0) {
        dr = 1;
        //MoterSpeed(1,MOTOSLEEP);
    } else if(num < 0){
        dr = 2;
        //MoterSpeed(2,MOTOSLEEP);
        num = - num;
    } else {
        MoterSpeed(3,0);
        return 0x80;
    }
    do {
        position_difference = num - MoterReadCodingSite();
        MoterSpeed(dr, (u8)((100 - (TimerGetSpeed() + sleep_sub) ) * SPEEDP));
        if(position_difference < 10) {
            sleep_sub = 40 - position_difference * 5;
        }
        current = MoterReadCurrent();
    } while(position_difference > 3);
    
    MoterSpeed(3,0);//stop
    
    return 0x80;
}
