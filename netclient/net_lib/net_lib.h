#pragma once

//enet

#define HAS_POLL
#define HAS_MSGHDR_FLAGS
#define HAS_SOCKLEN_T
#define HAS_FCNTL

#include <enet/callbacks.c>
#include <enet/compress.c>
#include <enet/host.c>
#include <enet/list.c>
#include <enet/packet.c>
#include <enet/peer.c>
#include <enet/protocol.c>

#ifdef _WIN32
    #include <enet/win32.c>
#else 
    #include <enet/unix.c>
#endif


//stuff

#include <net_lib/host.cpp>
#include <net_lib/export.cpp>
#include <net_lib/global.cpp>

#include <net_lib/common/net_peer.cpp>
#include <net_lib/common/packet_buffer.cpp>
#include <net_lib/common/message_handler.c>
#include <net_lib/common/net_time.c>

#include <net_lib/net.cpp>


#ifdef DC_CLIENT
#include <net_lib/common/pviz.c>
#endif
//#include <net_lib/common/sequencer.c>

//#include <net_lib/common/python_channel.cpp>

//#ifdef DC_CLIENT
//#include <net_lib/client/client.cpp>
//#include <net_lib/client.c>
//#endif

//#ifdef DC_SERVER
//#include <net_lib/server/server.c>
//#include <net_lib/server.c>
//#endif
