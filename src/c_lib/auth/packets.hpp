#pragma once

#include <net_lib/net_StoC.hpp>
#include <net_lib/net_CtoS.hpp>

#include <auth/constants.hpp>

namespace Auth
{

class auth_token_CtoS: public FixedSizeReliableNetPacketToServer<auth_token_CtoS>
{
    public:
        static const bool auth_required = false;

        char token[AUTH_TOKEN_LENGTH+1];

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_string(this->token, AUTH_TOKEN_LENGTH+1, buff, buff_n, pack);
    }

    inline void handle();
};

class auth_token_valid_StoC: public FixedSizeReliableNetPacketToClient<auth_token_valid_StoC>
{
    public:

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
    }

    inline void handle();
};

class auth_token_invalid_StoC: public FixedSizeReliableNetPacketToClient<auth_token_invalid_StoC>
{
    public:

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
    }

    inline void handle();
};

class clock_time_StoC: public FixedSizeReliableNetPacketToClient<clock_time_StoC>
{
    public:
        uint64_t server_time;

    inline void packet(char* buff, unsigned int* buff_n, bool pack)
    {
        pack_u64(&server_time, buff, buff_n, pack);
    }
    inline void handle();
};

}   // Auth
