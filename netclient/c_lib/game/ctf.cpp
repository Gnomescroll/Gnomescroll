#include "ctf.hpp"

CTF::CTF() {

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

void CTF::add_agent_to_team(int team, int agent) {
    if (team < 0 || team > 2) {
        printf("CTF::add_agent_to_team -- team id %d out of range\n", team);
        return;
    }

    Agent_state* a = STATE::agent_list.get(agent);
    if (a==NULL) return;

    if (a->status.team == team) return;    // already in team

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
            none.add_agent(agent);
            break;
        case 1:
            one.add_agent(agent);
            break;
        case 2:
            two.add_agent(agent);
            break;
        default:
            break;
    }

    a->status.team = team;
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
