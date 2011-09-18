#pragma once

#include "../common/net_time.h"
//#include "../common/net_peer.h"

#ifdef DC_CLIENT
	#include <compat_gl.h>
#endif

void pviz_start_frame();

void toggle_latency_unit();

void pviz_packet_sent(int seq, int size);
void pviz_packet_ack(int seq);

#ifdef DC_CLIENT
	void pviz_draw(float x, float y, float z);
	void pviz_packet_histrogram_draw(float x, float y, float z);
	void pviz_packet_histrogram2_draw(float x, float y, float z);
#endif
