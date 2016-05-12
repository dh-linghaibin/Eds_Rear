
#include "Control.h"
#include "Moter.h"
#include "Timer.h"
#include "Eeprom.h"
#include "Delay.h"

#define TRANDFORMATIONP 50

#define SPEEDP 2.5
                                // 0,2000,4500,6500,8500,10000,12000,14500,16500,19000,21000                 
const static u16 stalls_add[11] = {0,2200,4800,6800,8800,11000,13400,15700,18200,20500,23000}; //21000
static u16 stalls_start = 26000;  //19000
static u8  stalls = 0;

void ControlInit(void) {
    if(EepromRead(10) != 0x55) {
        EepromWrite(10, 0x55);
        EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
        EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
        EepromWrite(13, stalls);
        Write_Option_Byte();//设置使用第二功能
    }
    stalls_start = TypeCombinationU16(EepromRead(11), EepromRead(12));
    stalls = EepromRead(13);
}

/*冒泡排序*/
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
    stalls = 0;
    stalls_start = ConterResistancePositionFiltering();
    EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
    EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
    EepromWrite(13, stalls);
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

u8 ControlSetp(u16 num, u8 dr) {
    u16 position_difference = 0;//位置计算
    u16 current = 0,current_count = 0;//保护
    u8 sleep_sub = 0;//速度环
    MoterSetCodingSite(0);//clear
    do {
        position_difference = num - MoterReadCodingSite();
        MoterSpeed(dr, (u8)((100 - (TimerGetSpeed() + sleep_sub) ) * SPEEDP));
        if(position_difference < 10) {
            sleep_sub = 40 - position_difference * 5;
        }
        current = MoterReadCurrent();
        if(current > 65000) { //55000 新的值还没有经过测试
            if(current_count < 500) { //4000
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                //return 0x44; 还未经过测试
                while() {//等待恢复
                    current = MoterReadCurrent();
                }
            }
        } else {
            current_count = 0;
        }
    } while(position_difference > 3);
    MoterSpeed(3,0);//stop
    return 0x80;
}

u8 ControlRunPosition(int num) {
    u8 dr = 0;
    u16 setp_run = 0;
    MoterSetCodingSite(0);//clear
    if(num > 0) {
        dr = 1;
        //num += 10;
    } else if(num < 0) {//减档
        dr = 2;
        num = -num;
       // num -= 10;
        if(num < 0) {
            num = 0;
        }
    } else {
        MoterSpeed(3,0);
        ControlSetStall(stalls_fist);//suff
        return 0x80;
    }
    if(dr == 2) {
        setp_run = (u16)(num*0.7);
    } else {
        setp_run = num+4;
    }
    if(ControlSetp(setp_run, dr) == 0x44) {
        return 0x44;
    }
    if(dr == 2) {
        DelayMs(100);
        setp_run = (num - setp_run);
    } else {
        DelayMs(150);
        dr = 2;
        setp_run = 17;
    }
    if(ControlSetp(setp_run, dr) == 0x44) {
        return 0x44;
    }
    ControlSetStall(stalls_fist);//suff
    return 0x80;
}
