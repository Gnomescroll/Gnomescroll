#include "ctf.hpp"

#include <c_lib/agent/player_agent.hpp>
#include <c_lib/state/client_state.hpp>

void CTF::init() {
    none.init(0);
    one.init(1);
    two.init(2);

    one.set_color(10,10,10);    //black-ish
    two.set_color(10,210,10);   // green-ish

    char team_name_one[] = "Raiders";
    one.set_name(team_name_one);

    char team_name_two[] = "Ex-military";
    two.set_name(team_name_two);
}

#ifdef DC_CLIENT

void CTF::join_team(int team) {
    AgentJoinTeam_CtoS* msg = new AgentJoinTeam_CtoS(team, ClientState::playerAgent_state.agent_id);
    msg->send();
}

void CTF::on_ready() {
    if (!auto_assign) return;
    // choose the less populated team
    int team = (one.n > two.n) ? two.id : one.id;
    join_team(team);
}

#endif

#ifdef DC_SERVER

void CTF::on_client_connect(int client_id) {
    one.update_client(client_id);
    two.update_client(client_id);
}

#endif

bool CTF::add_agent_to_team(int team, int agent) {
    bool success = false;

    if (team < 0 || team > 2) {
        printf("CTF::add_agent_to_team -- team id %d out of range\n", team);
        return success;
    }

    Agent_state* a = STATE::agent_list.get(agent);
    if (a==NULL) return success;

    if (a->status.team == team) {
        return success;    // already in team
    }


    switch(a->status.team) {
        case 0:
            none.remove_agent(agent);
            break;
        case 1:
            one.remove_agent(agent);
            break;
        case 2:
            two.remove_agent(agent);
            break;
        default:
            break;
    }
    
    switch(team) {
        case 0:
            success = none.add_agent(agent);
            break;
        case 1:
            success = one.add_agent(agent);
            break;
        case 2:
            success = two.add_agent(agent);
            break;
        default:
            break;
    }

    a->status.team = team;
    return success;
}

void CTF::set_team_color(int team,
    unsigned char r, unsigned char g, unsigned char b) {
    if (team < 1 || team > 2) {
        printf("CTF::set_team_color -- team id %d out of range\n", team);
        return;
    }

    switch (team) {
        case 1:
            one.set_color(r,g,b);
            break;
        case 2:
            two.set_color(r,g,b);
            break;
        default:
            break;
    }
}
