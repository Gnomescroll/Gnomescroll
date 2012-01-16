#include "global.hpp"

namespace NetClient
{
    NetPeer Server;
}

namespace NetServer
{
    NetPeer* pool[HARD_MAX_CONNECTIONS];
}