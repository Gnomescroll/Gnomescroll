#ifndef net_lib_client_pviz_h
#define net_lib_client_pviz_h

#include "../common/net_time.h"
#include "../common/netpeer.h"

#ifdef DC_CLIENT
#include <compat_gl.h>

void pviz_start_frame();

void pviz_draw(float x, float y, float z);
#endif

#endif
