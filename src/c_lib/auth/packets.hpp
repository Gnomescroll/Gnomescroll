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

}   // Auth
