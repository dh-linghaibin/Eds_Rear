<<<<<<< HEAD

#ifndef CONTROL_H
#define CONTROL_H

#include "Type.h"

void ControlInit(void);
int ControlCalculateGrating(u8 stalss);
u8 ControlRunPosition(int num);

u8 ControlSetStallsSub(void);
u8 ControlSetStallsAdd(void);

u8 ControlGetStall(void);
void ControlSetStart(void);

u8 ControlSetp(u16 num, u8 dr);
u8 ControlSetpSmall(u16 num, u8 dr);
//ÉèÖÃµµÎ»
u8 ControlSetStallsSet(u8 set_stall);

/*Ã°ÅÝÅÅÐò*/
u16 ConterResistancePositionFiltering(void);

void ControlSetStalls(u8 num,u16 data);

void ControlSetStallsStart(u16 data);

void ControlEleSet(u16 time);
#endif

=======

#ifndef CONTROL_H
#define CONTROL_H

#include "Type.h"

void ControlInit(void);
int ControlCalculateGrating(u8 stalss);
u8 ControlRunPosition(int num);

u8 ControlSetStallsSub(void);
u8 ControlSetStallsAdd(void);

u8 ControlGetStall(void);
void ControlSetStart(void);

u8 ControlSetp(u16 num, u8 dr);

//ÉèÖÃµµÎ»
u8 ControlSetStallsSet(u8 set_stall);

/*Ã°ÅÝÅÅÐò*/
u16 ConterResistancePositionFiltering(void);

#endif

>>>>>>> origin/master
