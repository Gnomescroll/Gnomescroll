#pragma once


#include <net_lib/common/net_peer.hpp>

namespace NetClient
{
    extern NetPeer Server;
}

namespace NetServer
{
    const int HARD_MAX_CONNECTIONS = 64;

    extern NetPeer* pool[HARD_MAX_CONNECTIONS];
}