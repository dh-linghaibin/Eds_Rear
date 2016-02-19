
#include "Control.h"
#include "Moter.h"
#include "Timer.h"
#include "Eeprom.h"
#include "Delay.h"

#define TRANDFORMATIONP 50

#define SPEEDP 2.5
                                    //5000         10500
static u16 stalls_add[11] = {0,2000,4500,6500,8500,10000,12000,14500,16500,19000,21000}; //21000
static u16 stalls_start = 19000;
static u8  stalls = 0;

void ControlInit(void) {
    if(EepromRead(10) != 0x55) {
        EepromWrite(10, 0x55);
        EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
        EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
        EepromWrite(13, stalls);
    }
    stalls_start = TypeCombinationU16(EepromRead(11), EepromRead(12));
    stalls = EepromRead(13);
}

/*√∞≈›≈≈–Ú*/
u16 ConterResistancePositionFiltering(void) {
    u16 res[3] = {0,0,0};
    for(u8 i = 0; i < 3; i++) {
        res[i] = MoterReadResistancePosition();//get position
        DelayMs(100);
    }
    for(u8 i = 0; i < 3; i++) {
        for(u8 j = 0; i + j < 2; j++) {
            if(res[j] > res[j + 1]) {
                int temp = res[j];
                res[j] = res[j + 1];
                res[j + 1] = temp;
            }
        }
    }
    return res[1];
}

void ControlSetStart(void) {
    stalls_start = ConterResistancePositionFiltering();
    EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
    EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
}

static u8 stalls_fist = 0;

u8 ControlSetStallsSub(void) {
    if(stalls > 0) {
        stalls_fist = (stalls -1);
        return stalls_fist;
    } else {
        return 0x44;
    }
}

u8 ControlSetStallsAdd(void) {
    if(stalls < 10) {
        stalls_fist = (stalls + 1);
        return stalls_fist;
    } else {
        return 0x44;
    }
}

u8 ControlGetStall(void) {
    return stalls;
}

void ControlSetStall(u8 cmd) {
    stalls = cmd;
    EepromWrite(13, stalls);
}


int ControlCalculateGrating(u8 stalss) {
    u16 res_position_absolutely = 0;
    u16 res_position_new = 0;
    u16 res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
    res_position_absolutely = stalls_add[stalss] + stalls_start;//count position
    res_position_new = ConterResistancePositionFiltering();//get position
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
    return rotate_num;
}



u8 ControlRunPosition(int num) {
    u8 dr = 0;
    u16 position_difference = 0;
    u8 sleep_sub = 0;
    u16 current = 0,current_count = 0;
    MoterSetCodingSite(0);//clear
    if(num > 0) {
        dr = 1;
        num += 10;
    } else if(num < 0) {
        dr = 2;
        num = - num;

    } else {
        MoterSpeed(3,0);
        ControlSetStall(stalls_fist);//suff
        return 0x80;
    }
    do {
        position_difference = num - MoterReadCodingSite();
        MoterSpeed(dr, (u8)((100 - (TimerGetSpeed() + sleep_sub) ) * SPEEDP));
        if(position_difference < 10) {
            sleep_sub = 40 - position_difference * 5;
        }
        current = MoterReadCurrent();
        if(current > 55000) {
            if(current_count < 4000) {
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                return 0x44;
            }
        } else {
            current_count = 0;
        }
    } while(position_difference > 3);
    MoterSpeed(3,0);//stop
    DelayMs(800);
    if(dr == 1) {
        dr = 2;
    }
    MoterSpeed(dr,150);
    DelayMs(50);
    MoterSpeed(3,0);//stop
    ControlSetStall(stalls_fist);//suff
    return 0x80;
}
