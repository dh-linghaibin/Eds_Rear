
#include "Control.h"
#include "Moter.h"
#include "Timer.h"
#include "Eeprom.h"
#include "Delay.h"

#define TRANDFORMATIONP  60.8 //电阻转脉冲系数

#define SPEEDP 2.5
                                // 0,2000,4500,6500,8500,10000,12000,14500,16500,19000,21000                 
static const u16 stalls_add[11] = {0,4300,8400,11400,14200,17200,21100,24300,28500,33900,38000};//{0,2900,5300,7600,9900,12100,14300,16600,19200,21200,23200};//{0,2200,4800,6800,8800,11000,13400,15700,18200,20500,23000}; //21000
//static u16 stalls_add_error[11] = {0,4300,8400,11400,14200,17200,21100,24300,28500,33900,38000};
static u16 stalls_start = 26000;  //19000

static u8  stalls = 0;

static u16 ele_time = 700;

void ControlInit(void) {
    if(EepromRead(10) != 0x55) {
        EepromWrite(10, 0x55);
        EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
        EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
        EepromWrite(13, stalls);
        for(int i = 0;i < 11;i++) {
            EepromWrite(14+(i*2), TypeDecomposeU16(stalls_add[i], 0));
            EepromWrite(15+(i*2), TypeDecomposeU16(stalls_add[i], 1));
        }
        EepromWrite(40, TypeDecomposeU16(ele_time, 0));
        EepromWrite(41, TypeDecomposeU16(ele_time, 1));
       // Write_Option_Byte();//设置使用第二功能
    }
    stalls_start = TypeCombinationU16(EepromRead(11), EepromRead(12));
    stalls = EepromRead(13);
//    for(int i = 0;i < 11;i++) {
//        stalls_add[i] = TypeCombinationU16(EepromRead(14+(i*2)), EepromRead(15+(i*2)));
//    }
    ele_time  = 700;//TypeCombinationU16(EepromRead(40),EepromRead(41));
}

void ControlSetStalls(u8 num,u16 data) {
   // stalls_add[num] = data;
   // EepromWrite(14+(num*2), TypeDecomposeU16(stalls_add[num], 0));
   // EepromWrite(15+(num*2), TypeDecomposeU16(stalls_add[num], 1));
}

void ControlSetStallsStart(u16 data) {
    stalls = 1;
    stalls_start = data;
    EepromWrite(11, TypeDecomposeU16(stalls_start, 0));
    EepromWrite(12, TypeDecomposeU16(stalls_start, 1));
    EepromWrite(13, stalls);
}

void ControlEleSet(u16 time) {
    ele_time = time;
    EepromWrite(40, TypeDecomposeU16(ele_time, 0));
    EepromWrite(41, TypeDecomposeU16(ele_time, 1));
}

/*冒泡排序*/
u16 ConterResistancePositionFiltering(void) {
    u16 res[6] = {0,0,0,0,0,0};
    for(u8 i = 0; i < 6; i++) {
        res[i] = MoterReadResistancePosition();//get position
        DelayMs(100);
    }
    for(u8 i = 0; i < 6; i++) {
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
    stalls = 1;
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

//设置档位
u8 ControlSetStallsSet(u8 set_stall) {
    if(set_stall < 10) {
        return set_stall;
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

unsigned long xx2 = 0;
unsigned long xx3 = 0;

static u8 space_dr = 0;

int ControlCalculateGrating(u8 stalss_c) {
    unsigned long res_position_absolutely = 0;
    u16 res_position_new = 0;
    unsigned long res_position_different = 0;
    u8 symbol_bit = 0;
    int rotate_num = 0;
   
    res_position_new = ConterResistancePositionFiltering();//get position
    
    res_position_absolutely = stalls_add[stalss_c] + stalls_start;//count position
    
    xx2 = res_position_absolutely;
    if(res_position_absolutely > res_position_new) {
        symbol_bit = 0;
        res_position_different = res_position_absolutely - res_position_new;
    } else {
        symbol_bit = 1;
        res_position_different = res_position_new - res_position_absolutely;
    }
    xx3 = res_position_different;
    rotate_num = (u16)(res_position_different/TRANDFORMATIONP);
    if(stalls_fist > stalls) {
        if(space_dr == 0) {
            space_dr = 1;
        } else if(space_dr == 2) {
            rotate_num += 5;
            space_dr = 1; 
        }
    } else {
        if(space_dr == 0) {
            space_dr = 2;
        } else if(space_dr == 1) {
            rotate_num += 5;
            space_dr = 2;
        }
    }
    if(symbol_bit == 1) {
        rotate_num = 0 - rotate_num;
    }
    return rotate_num;
}

u16 eds_stop = 10;//多少脉冲开始刹车
u16 position_difference = 0;//位置计算
u8 sleep_sub = 0;//速度环

u16 need_setp = 0;//需要走的脉冲

u16 need_curr = 0;

u16 arraved = 0;//走到的位置


u8 ControlSetp(u16 num, u8 dr) {
    
    u16 current = 0,current_count = 0;//保护
    
    u16 time_count = 0;//超时时间
    
    need_setp = num;
    
    u8 sleep = 255;
    
    
    eds_stop = 10;
    position_difference = 0;
    
    MoterSetCodingSite(0);//clear
    do {
        if(num - MoterReadCodingSite() > 0) {
            position_difference = num - MoterReadCodingSite();
        } else {
            position_difference = 0;
        }
        MoterSpeed(dr,sleep);//电机控制
        sleep_sub = TimerGetSpeed();//速度
        current = MoterReadCurrent();//电流
        
        if(position_difference < 20) {
            need_curr = current;
            if(need_curr > 55000) {
                eds_stop = 0;//
            } else if(need_curr > 50000) {
                eds_stop = 1;//
            } else if(need_curr > 45000) {
                eds_stop = 2;//
            } else if(need_curr > 40000) {
                eds_stop = 3;//
            } else if(need_curr > 35000) {
                eds_stop = 4;//
            } else if(need_curr > 30000) {
                eds_stop = 5;//
            } else if(need_curr > 25000) {
                eds_stop = 6;//
            } else if(need_curr > 20000) {
                eds_stop = 7;//
            } else if(need_curr > 15000) {
                eds_stop = 8;//
            } else if(need_curr > 10000) {
                eds_stop = 9;//9
            } else if(need_curr > 8500) {
                eds_stop = 10;//10
            } else {
                eds_stop = 15;
            }
        }
        
        if(current > 51000) { //55000 新的值还没有经过测试   过流电流值
            if(current_count < 12000) { //4000  过流时间
                current_count++;
            } else {
                MoterSpeed(3,0);//stop
                return 0x44; //还未经过测试
  //              while() {//等待恢复
//                    current = MoterReadCurrent();
//                }
            }
        } else {
            current_count = 0;
        }
        //超时时间统计
        if(time_count < 20000) {
            time_count++;
        } else {
            //超时退出
            time_count = 0;
            MoterSpeed(3,0);//stop
            return 0x44; //还未经过测试
        }
    } while(position_difference > eds_stop);
    MoterSpeed(3,0);//stop
    DelayMs(100);
    return 0x80;
}

u8 ControlSetpSmall(u16 num, u8 dr) {
    u16 position_difference = 0;//位置计算
    u8 sleep = 255;
    MoterSetCodingSite(0);//clear
    do {
        if(num - MoterReadCodingSite() > 0) {
            position_difference = num - MoterReadCodingSite();
        } else {
            position_difference = 0;
        }
        MoterSpeed(dr,sleep);
    } while(position_difference > 1);  //10/95  12/65  11
    MoterSpeed(3,0);//stop
    DelayMs(100);
    return 0x80;
}


u8 ControlRunPosition(int num) {
    u8 dr = 0;
    u16 setp_run = 0;
    MoterSetCodingSite(0);//clear
    if(num > 0) {
        dr = 1;
    } else if(num < 0) {//减档
        dr = 2;
        num = -num;

        if(num < 0) {
            num = 0;
        }
    } else {
        MoterSpeed(3,0);
        ControlSetStall(stalls_fist);//suff
        return 0x80;
    }
    setp_run = num;//-20;
    if(ControlSetp(setp_run, dr) == 0x44) {
        return 0x44;
    }
    ControlSetStall(stalls_fist);//suff
    return 0x80;
}