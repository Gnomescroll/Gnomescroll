#pragma once

typedef enum
{
    DISCONNECT_NONE = 0,        // enet sets it as 0 for its internal disconnects
    DISCONNECT_TIMEOUT,         // connection timeout
    DISCONNECT_GRACEFULLY,      // normal disconnect
    DISCONNECT_FULL,            // server was full
    DISCONNECT_FORCED,          // generic forced disconnect
    DISCONNECT_BAD_PACKET,      // client sent malformed packets
    DISCONNECT_AUTH_EXPIRED,    // client's authorization expired
    DISCONNECT_AUTH_TIMEOUT,    // client failed to authorize within time limit after connecting
    DISCONNECT_SERVER_ERROR,    // some error occured in the server
    DISCONNECT_LOGIN_ELSEWHERE, // user logged in with a new connection
}   DisconnectType;
