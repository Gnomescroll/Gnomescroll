#pragma once

#ifdef DC_CLIENT

//message handling
//#include <agent/control_state.h>
//#include <agent/agent_client.h>

/*
 #ifdef __cplusplus
 extern "C" {
 #endif

 #ifdef __cplusplus
 }
 #endif
*/

void _NetClientConnect(int a, int b,int c, int d, int _port);
//void _NetClientTick();


void _NetClientStartFrame();
void _NetClientStateTick();
void _NetClientNetInTick();
void _NetClientNetOutTick();

#endif
