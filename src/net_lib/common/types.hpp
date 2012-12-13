#pragma once

#include <net_lib/global.hpp>

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
    DISCONNECT_AUTH_LIMIT,      // client sent too many bad token 
}   DisconnectType;

typedef enum
{
    NULL_USER_ID = 0
} UserID;

typedef enum
{
    NULL_CLIENT = 0xff,
} ClientID;

inline bool isValid(UserID user_id)
{
    return (user_id > 0);
}

inline bool isValid(ClientID client_id)
{
    return (client_id != NULL_CLIENT && client_id >= 0 && client_id < HARD_MAX_CONNECTIONS);
}
