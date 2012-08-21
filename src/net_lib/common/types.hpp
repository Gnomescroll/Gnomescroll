#pragma once

typedef enum
{
    DISCONNECT_NONE = 0,    // enet sets it as 0 for its internal disconnects
    DISCONNECT_TIMEOUT,
    DISCONNECT_GRACEFULLY,
    DISCONNECT_FULL,
    DISCONNECT_FORCED
}   DisconnectType;
