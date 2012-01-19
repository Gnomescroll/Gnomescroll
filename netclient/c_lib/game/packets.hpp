#pragma once

#include <net_lib/net.hpp>
#include <net_lib/global.hpp>
#include <net_lib/export.hpp>

/*
    Packet that handles startup
*/

class SendClientId_StoC: public FixedSizeReliableNetPacketToClient<SendClientId_StoC>
{
    public:
        int client_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&client_id, buff, buff_n, pack);
        }
        inline void handle()
        {
            //printf("Client Received Client id = %i \n", client_id);
            NetClient::Server.client_id = client_id;
            client_connect_event(client_id);
        }
};

/* Server -> Client */

class TeamColor_StoC: public FixedSizeReliableNetPacketToClient<TeamColor_StoC>
{
    public:
        int team;
        unsigned char r,g,b;

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
        int team;
        int agent;

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
        int team;
        int agent; // needs to be inferred from connection object!!!!

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_u8(&agent, buff, buff_n, pack);
        }
        inline void handle();
};

class AgentAutoAssignTeam_CtoS: public FixedSizeReliableNetPacketToServer<AgentAutoAssignTeam_CtoS>
{
    public:
        int agent;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&agent, buff, buff_n, pack);
        }
        inline void handle();
};

// Needs: string packing
//class TeamName_StoC: public FixedSizeReliableNetPacketToClient<TeamName_StoC>
//{
    //public:
        //int team;
        //char name[TEAM_NAME_MAX_LENGTH];

        //inline void packet(char* buff, int* buff_n, bool pack)
        //{
            //int i;
            //for (i=0; i<TEAM_NAME_MAX_LENGTH && name[i] != '\0'; i++)
                //pack_u8(name[i], buff, buff_n, pack);
            //// dont copy null terminator
        //}
        //inline void handle();

        //TeamName_StoC(){}
        //TeamName_StoC(int team, char* name)
        //: team(team)
        //{
            //int i;
            //for (i=0; i<TEAM_NAME_MAX_LENGTH && i != '\0'; i++) {
                //this->name[i] = name[i];
            //}
            //// dont copy null terminator
        //}
//};

class TeamScore_StoC: public FixedSizeReliableNetPacketToServer<TeamScore_StoC>
{
    public:
        int team;
        int score;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&team, buff, buff_n, pack);
            pack_u8(&score, buff, buff_n, pack);
        }
        inline void handle();
};


/* Items */

class FlagState_StoC: public FixedSizeReliableNetPacketToClient<FlagState_StoC>
{
    public:
        int team;
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
        int team;
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
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};
