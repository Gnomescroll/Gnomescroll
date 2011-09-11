#ifndef net_lib_client_pviz_h
#define net_lib_client_pviz_h

#include "../common/net_time.h"
//#include "../common/net_peer.h"

#ifdef DC_CLIENT
#include <compat_gl.h>

void pviz_start_frame();

void pviz_draw(float x, float y, float z);
void pviz_packet_histrogram_draw(float x, float y, float z);

void pviz_packet_sent(int seq, int size);
void pviz_packet_ack(int seq);
#endif

#endif
