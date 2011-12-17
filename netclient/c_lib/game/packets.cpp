#include "packets.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>

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

// dummies
inline void AgentJoinTeam_CtoS::handle(){}
inline void AgentAutoAssignTeam_CtoS::handle(){}

#endif

#ifdef DC_SERVER

// dummies
inline void TeamColor_StoC::handle(){}
inline void AgentJoinTeam_StoC::handle(){}
//inline void TeamName_StoC::handle(){}

inline void AgentJoinTeam_CtoS::handle() {
    bool added = ServerState::ctf.add_agent_to_team(team, agent);
    if (added) {
        AgentJoinTeam_StoC* msg = new AgentJoinTeam_StoC(team, agent);
        msg->broadcast();
    }
}

inline void AgentAutoAssignTeam_CtoS::handle() {
    ServerState::ctf.auto_assign_agent(agent);
}


#endif
