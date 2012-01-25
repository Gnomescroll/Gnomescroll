#include "ctf.hpp"


#include <c_lib/agent/player_agent.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/game/packets.hpp>
#include <common/random.h>

CTF::CTF()
{}

void CTF::init() {
    one.set_color(10,10,200);    //black-ish
    two.set_color(10,210,10);   // green-ish

    none.init(0);
    one.init(1);
    two.init(2);
}

void CTF::start() {
    float x,y,z;

    // TODO use real map widths
    int x_max = 128;
    int y_max = 128;

    x = randrange(0, x_max-1);
    y = randrange(0, (y_max-1)/2);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_base_position(1, x,y,z);
    x = randrange(0, x_max-1);
    y = randrange(0, (y_max-1)/2);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_flag_position(1, x,y,z);

    x = randrange(0, x_max-1);
    y = randrange((y_max-1)/2, y_max-1);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_base_position(2,x,y,z);
    x = randrange(0, x_max-1);
    y = randrange((y_max-1)/2, y_max-1);
    z = _get_highest_open_block(x,y);
    x += randf();y += randf();
    this->set_flag_position(2,x,y,z);
}

void CTF::set_team_name(int team, char* name)
{
    switch (team)
    {
        case 1:
            one.set_name(name);
            break;
        case 2:
            two.set_name(name);
            break;
        default:
            printf("CTF::set_team_name -- invalid team %d\n", team);
            return;
    }
}

void CTF::set_score(int team, int score)
{
    switch (team) {
        case 1:
            one.base_score = score;
            break;
        case 2:
            two.base_score = score;
            break;
        default:
            printf("CTF::set_score, team %d invalid\n", team);
            return;
    }
}

void CTF::set_flag_position(int team, float x, float y, float z)
{
    switch (team) {
        case 1:
            one.flag->set_position(x,y,z);
            break;
        case 2:
            two.flag->set_position(x,y,z);
            break;
        default:
            break;
    }

    #ifdef DC_SERVER
    FlagState_StoC msg;
    msg.team = team;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    #endif
}

void CTF::set_base_position(int team, float x, float y, float z)
{
    switch (team) {
        case 1:
            one.base->set_position(x,y,z);
            break;
        case 2:
            two.base->set_position(x,y,z);
            break;
        default:
            break;
    }

    #ifdef DC_SERVER
    BaseState_StoC msg;
    msg.team = team;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    #endif
}

#ifdef DC_CLIENT

void CTF::join_team(int team) {
    AgentJoinTeam_CtoS msg;
    msg.team = team;
    msg.agent = ClientState::playerAgent_state.agent_id;
    msg.send();
}

void CTF::on_ready() {
    static int once = 0;
    if (once) return;
    if (!auto_assign) return;
    AgentAutoAssignTeam_CtoS msg;
    msg.agent = ClientState::playerAgent_state.agent_id;
    msg.send();
    once = 1;
}

void CTF::flag_picked_up(int team)
{
    switch (team)
    {
        case 1:
            if (two.flag->vox != NULL)
                two.flag->vox->set_draw(false);
            break;
        case 2:
            if (one.flag->vox != NULL)
                one.flag->vox->set_draw(false);
            break;
        default:
            printf("CTF::flag_picked_up -- invalid team %d\n", team);
            return;
    }
}

void CTF::flag_dropped(int team)
{
    switch (team)
    {
        case 1:
            if (two.flag->vox != NULL)
                two.flag->vox->set_draw(true);
            break;
        case 2:
            if (one.flag->vox != NULL)
                one.flag->vox->set_draw(true);
            break;
        default:
            printf("CTF::flag_dropped -- invalid team %d\n", team);
            return;
    }
}

void CTF::flag_scored(int team)
{
    switch (team)
    {
        case 1:
            if (two.flag->vox != NULL)
                two.flag->vox->set_draw(true);
            break;
        case 2:
            if (one.flag->vox != NULL)
                one.flag->vox->set_draw(true);
            break;
        default:
            printf("CTF::flag_scored -- invalid team %d\n", team);
            return;
    }
}

int CTF::get_team_color(int team, unsigned char *r, unsigned char *g, unsigned char *b)
{
    switch (team)
    {
        case 1:
            *r = one.r;
            *g = one.g;
            *b = one.b;
            break;
        case 2:
            *r = two.r;
            *g = two.g;
            *b = two.b;
            break;
        default:
            //printf("CTF::get_team_color -- invalid team %d\n", team);
            return 1;
    }
    return 0;
}
#endif

#ifdef DC_SERVER

void CTF::on_client_connect(int client_id) {
    one.update_client(client_id);
    two.update_client(client_id);
}

void CTF::auto_assign_agent(int agent_id) {
    // choose the less populated team
    CTFTeam* team = (one.n > two.n) ? &two : &one;
    bool added = false;
    if (!team->full()) {
        added = add_agent_to_team(team->id, agent_id);
    }
    if (added) {
        AgentJoinTeam_StoC msg;
        msg.team = team->id;
        msg.agent = agent_id;
        msg.broadcast();
    }
}

void CTF::send_to_client(int client_id)
{
    // send scores TODO

    // send flag/base states
    Flag* flag;
    Base* base;
    char *name;
    int i;
    for (i=1; i<=2; i++) {
        switch (i) {
            case 1:
                flag = one.flag;
                base = one.base;
                name = one.name;
                break;
            case 2:
                flag = two.flag;
                base = two.base;
                name = two.name;
                break;
            default:
                break;
        }
        FlagState_StoC flag_msg;
        flag_msg.team = i;
        flag_msg.x = flag->x;
        flag_msg.y = flag->y;
        flag_msg.z = flag->z;
        flag_msg.sendToClient(client_id);
        
        BaseState_StoC base_msg;
        base_msg.team = i;
        base_msg.x = base->x;
        base_msg.y = base->y;
        base_msg.z = base->z;
        base_msg.sendToClient(client_id);

        TeamName_StoC name_msg;
        name_msg.team = i;
        strcpy(name_msg.name, name);
        printf("sending team name %s\n", name_msg.name);
        name_msg.sendToClient(client_id);
    }
}

void CTF::reset_flag(int team)
{
    float x,y,z;
    int x_max = 128;
    int y_max = 128;

    x = randrange(0, x_max-1);
    switch (team)
    {
        case 1:
            y = randrange(0, (y_max-1)/2);
            break;
        case 2:
            y = randrange((y_max-1)/2, y_max-1);
            break;
        default:
            printf("CTF::reset_flag WARNING -- team %d invalid\n", team);
            return;
    }
    z = _get_highest_open_block(x,y);
    x += randf(); y += randf();
    this->set_flag_position(team, x,y,z);
}

void CTF::agent_drop_flag(int agent_team, float x, float y, float z)
{
    switch (agent_team)
    {
        case 1:
            this->set_flag_position(2, x,y,z);
            break;
        case 2:
            this->set_flag_position(1, x,y,z);
            break;
        default:
            printf("CTF::agent_drop_flag -- agent_team %d invalid.\n", agent_team);
            return;
    }
}

void CTF::check_agent_proximities() {
    float r;
    Flag* f;
    Base* b;
    Agent_state* a;
    int i;
        
    f = one.flag;
    r = f->vox->largest_radius();
    STATE::agent_list.agents_within_sphere(f->x, f->y, f->z, r);
    STATE::agent_list.sort_filtered_agents_by_distance();
    for (i=0; i<STATE::agent_list.n_filtered; i++) {
        a = STATE::agent_list.filtered_agents[i];
        if (a==NULL) continue;
        if (!a->status.team || a->status.team == 1) continue;
        // pickup
        if (!a->status.dead)
        {
            a->status.pickup_flag();
        }
        break;
    }

    b = one.base;
    r = b->vox->largest_radius();
    STATE::agent_list.agents_within_sphere(b->x, b->y, b->z, r);
    for (i=0; i<STATE::agent_list.n_filtered; i++) {
        a = STATE::agent_list.filtered_agents[i];
        if (a==NULL) continue;
        if (!a->status.team || a->status.team != 1) continue;
        // heal
        if (!a->status.dead) {
            if (a->status.has_flag) {
                // scored
                this->reset_flag(2);
                one.captured_flag();
                a->status.score_flag();
            }
            a->status.restore_health();
            a->weapons.restore_ammo();
        }
    }

    f = two.flag;
    r = f->vox->largest_radius();
    STATE::agent_list.agents_within_sphere(f->x, f->y, f->z, r);
    STATE::agent_list.sort_filtered_agents_by_distance();
    for (i=0; i<STATE::agent_list.n_filtered; i++) {
        a = STATE::agent_list.filtered_agents[i];
        if (a==NULL) continue;
        if (!a->status.team || a->status.team == 2) continue;
        // pickup
        if (!a->status.dead)
        {
            a->status.pickup_flag();
        }
        break;
    }

    b = two.base;
    r = b->vox->largest_radius();
    STATE::agent_list.agents_within_sphere(b->x, b->y, b->z, r);
    for (i=0; i<STATE::agent_list.n_filtered; i++) {
        a = STATE::agent_list.filtered_agents[i];
        if (a==NULL) continue;
        if (!a->status.team || a->status.team != 2) continue;
        // heal
        if (!a->status.dead) {
            if (a->status.has_flag) {
                // scored
                this->reset_flag(1);
                two.captured_flag();
                a->status.score_flag();
            }
            a->status.restore_health();
            a->weapons.restore_ammo();
        }
    }

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

    //a->status.team = team;
    a->status.set_team(team);
    printf("Added agent %d to team %d\n", agent, team);
    return success;
}

void CTF::set_team_color(int team,
    unsigned char r, unsigned char g, unsigned char b)
{
    switch (team) {
        case 1:
            one.set_color(r,g,b);
            break;
        case 2:
            two.set_color(r,g,b);
            break;
        default:
            printf("CTF::set_team_color -- invalid team %d\n", team);
            return;
    }
}

