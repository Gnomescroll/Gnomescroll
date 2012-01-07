#pragma once

#ifdef DC_CLIENT

void _NetClientConnect(int a, int b,int c, int d, int _port);

void _NetClientStartFrame();
void _NetClientStateTick();
void _NetClientNetInTick();
void _NetClientNetOutTick();

#endif
