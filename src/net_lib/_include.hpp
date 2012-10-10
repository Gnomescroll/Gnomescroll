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

#include <net_lib/common/types.hpp>

#include <net_lib/host.cpp>
#include <net_lib/global.cpp>

#include <net_lib/common/net_peer.cpp>
#include <net_lib/common/packet_buffer.cpp>
#include <net_lib/common/message_handler.c>
#include <net_lib/common/net_time.c>

#include <net_lib/common/packet_id_counter.cpp>
#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

#if DC_SERVER
#include <net_lib/common/net_peer_manager.cpp>
#include <net_lib/server.cpp>
#endif

#if DC_CLIENT
//#include <net_lib/benchmark/pviz.c>
#include <net_lib/client.cpp>
#endif
