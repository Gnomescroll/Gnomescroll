#include "packets.hpp"

#ifdef DC_CLIENT

inline void TeamColor_StoC::handle(){
    ClientState::ctf.set_team_color(team, r,g,b);
}

inline void AgentJoinTeam_StoC::handle() {
    ClientState::ctf.add_agent_to_team(team, agent);
}

//inline void TeamName_StoC::handle() {
    //ClientState::CTF.set_team_name(team, name);
//}

inline void TeamScore_StoC::handle() {
    ClientState::ctf.set_score(team, score);
}

inline void FlagState_StoC::handle()
{
    ClientState::ctf.set_flag_position(team, x,y,z);
}

inline void BaseState_StoC::handle()
{
    ClientState::ctf.set_base_position(team, x,y,z);
}

inline void AgentPickupFlag_StoC::handle()
{
    
}

// dummies
inline void AgentJoinTeam_CtoS::handle(){}
inline void AgentAutoAssignTeam_CtoS::handle(){}

#endif

#ifdef DC_SERVER

// dummies
inline void TeamColor_StoC::handle(){}
inline void AgentJoinTeam_StoC::handle(){}
//inline void TeamName_StoC::handle(){}
inline void TeamScore_StoC::handle() {}
inline void FlagState_StoC::handle() {}
inline void BaseState_StoC::handle() {}
inline void AgentPickupFlag_StoC::handle() {}

inline void AgentJoinTeam_CtoS::handle() {
    bool added = ServerState::ctf.add_agent_to_team(team, agent);
    if (added) {
        AgentJoinTeam_StoC msg;
        msg.team = team;
        msg.agent = agent;
        msg.broadcast();
    }
}

inline void AgentAutoAssignTeam_CtoS::handle() {
    ServerState::ctf.auto_assign_agent(agent);
}

#endif
