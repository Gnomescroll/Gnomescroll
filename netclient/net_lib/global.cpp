#include "global.hpp"

namespace NetClient
{
    extern NetPeer Server;
}

namespace NetServer
{
    extern NetPeer* pool[HARD_MAX_CONNECTIONS];
}