#pragma once

#include <net_lib/global.hpp>

#include <game/teams.hpp>

/* Server -> Client */

class TeamColor_StoC: public FixedSizeReliableNetPacketToClient<TeamColor_StoC>
{
    public:
        uint8_t team;
        uint8_t r,g,b;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_u8(&r, buff, buff_n, pack);
            pack_u8(&g, buff, buff_n, pack);
            pack_u8(&b, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentJoinTeam_StoC: public FixedSizeReliableNetPacketToClient<AgentJoinTeam_StoC>
{
    public:
        uint8_t team;
        uint8_t agent;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_u8(&agent, buff, buff_n, pack);
        }
        inline void handle();
};

/* Client -> Server */

class AgentJoinTeam_CtoS: public FixedSizeReliableNetPacketToServer<AgentJoinTeam_CtoS>
{
    public:
        uint8_t team;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentAutoAssignTeam_CtoS: public FixedSizeReliableNetPacketToServer<AgentAutoAssignTeam_CtoS>
{
    public:
        inline void packet(char* buff, int* buff_n, bool pack)
        {}
        inline void handle();
};

class TeamName_StoC: public FixedSizeReliableNetPacketToClient<TeamName_StoC>
{
    public:
        uint8_t team;
        char name[TEAM_NAME_MAX_LENGTH+1];

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_string(name, TEAM_NAME_MAX_LENGTH+1, buff, buff_n, pack);
        }
        inline void handle();
};

class TeamScore_StoC: public FixedSizeReliableNetPacketToClient<TeamScore_StoC>
{
    public:
        uint8_t team;
        uint16_t score;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_u16(&score, buff, buff_n, pack);
        }
        inline void handle();
};


/* Items */

class FlagState_StoC: public FixedSizeReliableNetPacketToClient<FlagState_StoC>
{
    public:
        uint8_t team;
        float x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class BaseState_StoC: public FixedSizeReliableNetPacketToClient<BaseState_StoC>
{
    public:
        uint8_t team;
        float x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentPickupFlag_StoC: public FixedSizeReliableNetPacketToClient<AgentPickupFlag_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentDropFlag_StoC: public FixedSizeReliableNetPacketToClient<AgentDropFlag_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentScoreFlag_StoC: public FixedSizeReliableNetPacketToClient<AgentScoreFlag_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};
