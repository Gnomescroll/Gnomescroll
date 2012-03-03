#include "packets.hpp"

#ifdef DC_CLIENT
#include <c_lib/chat/client.hpp>

inline void TeamColor_StoC::handle()
{
    ClientState::ctf->set_team_color(team, r,g,b);
}

inline void AgentJoinTeam_StoC::handle() {
    Agent_state* a = ClientState::agent_list->get(agent);
    if (a==NULL) return;
    if(ClientState::ctf->add_agent_to_team(team, agent))
    {
        a->event.joined_team(team);
    }
}

inline void TeamName_StoC::handle() {
    ClientState::ctf->set_team_name(team, name);
}

inline void TeamScore_StoC::handle() {
    ClientState::ctf->set_score(team, score);
}

inline void FlagState_StoC::handle()
{
    ClientState::ctf->set_flag_position(team, x,y,z);
}

inline void BaseState_StoC::handle()
{
    ClientState::ctf->set_base_position(team, x,y,z);
}

inline void AgentPickupFlag_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a==NULL) return;
    a->event.picked_up_flag();
    ClientState::ctf->flag_picked_up(a->status.team);
    system_message->agent_pickup_flag(a);
}

inline void AgentDropFlag_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a==NULL) return;    
    a->event.dropped_flag();
    ClientState::ctf->flag_dropped(a->status.team);
    system_message->agent_drop_flag(a);
}

inline void AgentScoreFlag_StoC::handle()
{
    Agent_state* a = ClientState::agent_list->get(id);
    if (a==NULL) return;
    a->event.scored_flag();
    ClientState::ctf->flag_scored(a->status.team);
    system_message->agent_score_flag(a);
}

// dummies
inline void AgentJoinTeam_CtoS::handle(){}
inline void AgentAutoAssignTeam_CtoS::handle(){}

#endif

#ifdef DC_SERVER

// dummies
inline void TeamColor_StoC::handle(){}
inline void AgentJoinTeam_StoC::handle(){}
inline void TeamName_StoC::handle(){}
inline void TeamScore_StoC::handle() {}
inline void FlagState_StoC::handle() {}
inline void BaseState_StoC::handle() {}
inline void AgentPickupFlag_StoC::handle() {}
inline void AgentDropFlag_StoC::handle() {}
inline void AgentScoreFlag_StoC::handle() {}

inline void AgentJoinTeam_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;
    bool added = ServerState::ctf->add_agent_to_team(team, a->id);
    if (added) {
        AgentJoinTeam_StoC msg;
        msg.team = team;
        msg.agent = a->id;
        msg.broadcast();
    }
}

inline void AgentAutoAssignTeam_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;
    ServerState::ctf->auto_assign_agent(a->id);
}

#endif
