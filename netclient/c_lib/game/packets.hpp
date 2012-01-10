#pragma once

#include <net_lib/net.hpp>

/* Server -> Client */

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
            printf("Client Received Client id = %i \n", client_id);
        }
};


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

        TeamColor_StoC(){}
        TeamColor_StoC(int team,
            unsigned char r, unsigned char g, unsigned char b)
        : team(team), r(r), g(g), b(b)
        {}
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

        AgentJoinTeam_StoC(){}
        AgentJoinTeam_StoC(int team, int agent)
        : team(team), agent(agent)
        {}
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

        AgentJoinTeam_CtoS(){}
        AgentJoinTeam_CtoS(int team, int agent)
        : team(team), agent(agent)
        {}
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

        AgentAutoAssignTeam_CtoS(){}
        AgentAutoAssignTeam_CtoS(int agent)
        : agent(agent)
        {}
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

        TeamScore_StoC(){}
        TeamScore_StoC(int team, int score)
        : team(team), score(score)
        {}
};
            
