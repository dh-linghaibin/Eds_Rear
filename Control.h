
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

#endif

