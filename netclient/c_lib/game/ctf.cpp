#include "ctf.hpp"

#include <t_map/t_map.hpp>
#include <game/packets.hpp>
#include <common/random.h>
#include <options/options.hpp>

#ifdef DC_CLIENT
#include <state/client_state.hpp>
#include <hud/map.hpp>
#include <hud/compass.hpp>
#endif

CTF::CTF()
{}

void CTF::init() {
    //this->set_team_color(1, 34, 144, 191);    //dark teal
    //this->set_team_color(1, 93, 158, 24);    //dark green
    this->set_team_color(1, 59, 99, 17);    //dark green
    this->set_team_color(2, 158, 24, 93);   // darker red/purple

    none.init(0);
    one.init(1);
    two.init(2);
}

void CTF::start() {

    #ifdef DC_CLIENT
    this->auto_assign = Options::auto_assign_team;
    #endif

    #ifdef DC_SERVER
    float x,y,z;

    int x_max = map_dim.x;
    int y_max = map_dim.y;
    
    // bases
    // base should spawn in the back 2/16 or 7/16 strip of the side it belongs on
    // also shouldnt spawn within 1/8 of edge
    x = randrange(map_dim.x / 8, (map_dim.x * 7)/8 - 1); // dont spawn within 1/8 of edge, in x
    y = randrange((map_dim.y * 1)/16, (map_dim.y * 5)/16 - 1); // 2/16 - 5/16
    z = _get_highest_open_block(x,y);
    x+=0.5f;y+=0.5f;
    this->set_base_position(1, x,y,z);

    x = randrange(map_dim.x / 8, (map_dim.x * 7)/8 - 1); // dont spawn within 1/8 of edge, in x
    //y = randrange((map_dim.y * 11)/16, (map_dim.y * 15)/16 - 1);  // 11/16 - 15/16
    y = map_dim.y - y;  // keep y distance equal, for balance
    z = _get_highest_open_block(x,y);
    x+=0.5f;y+=0.5f;
    this->set_base_position(2,x,y,z);

    // flags
    x = randrange(0, x_max-1);
    y = randrange(0, (y_max * 3) / 8 - 1);
    z = _get_highest_open_block(x,y);
    x+=0.5f;y+=0.5f;
    this->set_flag_position(1, x,y,z);

    x = randrange(0, x_max-1);
    y = randrange((y_max * 5)/8, y_max-1);
    z = _get_highest_open_block(x,y);
    x+=0.5f;y+=0.5f;
    this->set_flag_position(2,x,y,z);

    this->set_team_name(1, Options::team_name_one);
    this->set_team_name(2, Options::team_name_two);
    #endif
}

void CTF::tick()
{
    one.tick();
    two.tick();
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

char* CTF::get_team_name(int team)
{
    static const char dummy[] = "";
    char* name = NULL;
    switch (team)
    {
        case 1:
            name = one.name;
            break;
        case 2:
            name = two.name;
            break;
        default:
            printf("CTF::get_team_name -- invalid team %d\n", team);
            break;
    }
    if (name == NULL) return ((char*)dummy);
    return name;
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

int CTF::get_score(int team)
{
    switch (team)
    {
        case 1:
            return one.score();
            break;
        case 2:
            return two.score();
            break;
        default:
            printf("CTF::get_team_score -- invalied team %d\n", team);
            break;
    }
    return 0;
}

void CTF::set_flag_position(int team, float x, float y, float z)
{
    // sanitize position
    if (x < 0) x = 0;
    if (x >= map_dim.x) x = map_dim.x - 1;
    if (y < 0) y = 0;
    if (y >= map_dim.y) y = map_dim.y - 1;
    if (z < 0) z = 0;
    if (z >= map_dim.z) z = map_dim.z - 1;
    
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
    // TODO -- undo
    //////////////
    x = 0.0f;
    y = 0.0f;
    //////////////
    
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
    msg.send();
}

void CTF::on_ready() {
    static int once = 0;
    if (once) return;
    if (!auto_assign) return;
    AgentAutoAssignTeam_CtoS msg;
    msg.send();
    once = 1;
}

void CTF::flag_picked_up(int team)
{
    switch (team)
    {
        case 1:
            if (two.flag->vox != NULL)
                two.flag->held = true;
            break;
        case 2:
            if (one.flag->vox != NULL)
                two.flag->held = true;
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
                two.flag->held = false;
            break;
        case 2:
            if (one.flag->vox != NULL)
                two.flag->held = false;
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
            if (two.flag != NULL && two.flag->vox != NULL)
                two.flag->held = false;
            break;
        case 2:
            if (two.flag != NULL && one.flag->vox != NULL)
                two.flag->held = false;
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

int CTF::get_enemy_team_color(int team, unsigned char *r, unsigned char *g, unsigned char *b)
{
    if (team)
        team = (team == 1) ? 2 : 1;
    return get_team_color(team, r,g,b);
}

void CTF::animate_flags()
{
    if (one.flag != NULL && one.flag->vox != NULL)
        one.flag->animate();
    if (two.flag != NULL && two.flag->vox != NULL)
        two.flag->animate();
}

void CTF::register_items_for_hitscan()
{
    one.flag->vox->set_hitscan(true);
    two.flag->vox->set_hitscan(true);
    one.base->vox->set_hitscan(true);
    two.base->vox->set_hitscan(true);

    one.flag->vox->register_hitscan();
    two.flag->vox->register_hitscan();
    one.base->vox->register_hitscan();
    two.base->vox->register_hitscan();

}
void CTF::unregister_items_for_hitscan()
{
    one.flag->vox->set_hitscan(false);
    two.flag->vox->set_hitscan(false);
    one.base->vox->set_hitscan(false);
    two.base->vox->set_hitscan(false);

    one.flag->vox->unregister_hitscan();
    two.flag->vox->unregister_hitscan();
    one.base->vox->unregister_hitscan();
    two.base->vox->unregister_hitscan();
}
#endif

#ifdef DC_SERVER

void CTF::on_client_connect(int client_id)
{
    one.update_client(client_id);
    two.update_client(client_id);
}

void CTF::auto_assign_agent(int agent_id)
{
    // choose the less populated team
    //CTFTeam* team = (one.n > two.n) ? &two : &one;
    CTFTeam* team = &one;   // TODO -- ren-enable teams
    bool added = false;
    if (!team->full())
    {
        added = add_agent_to_team(team->id, agent_id);
    }
    if (added)
    {
        AgentJoinTeam_StoC msg;
        msg.team = team->id;
        msg.agent = agent_id;
        msg.broadcast();
    }
}

void CTF::send_to_client(int client_id)
{
    // send flag/base states, scores, team names
    Flag* flag;
    Base* base;
    char *name;
    int score;
    int i;
    for (i=1; i<=2; i++) {
        switch (i) {
            case 1:
                flag = one.flag;
                base = one.base;
                name = one.name;
                score = two.score();
                break;
            case 2:
                flag = two.flag;
                base = two.base;
                name = two.name;
                score = two.score();
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
        name_msg.sendToClient(client_id);

        TeamScore_StoC score_msg;
        score_msg.team = i;
        score_msg.score = score;
        score_msg.sendToClient(client_id);
    }
}

void CTF::reset_flag(int team)
{
    float x,y,z;
    int x_max = map_dim.x;
    int y_max = map_dim.y;

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
    this->set_flag_position(team, x,y,z);
}

void CTF::agent_drop_flag(int agent_team, float x, float y, float z)
{
    switch (agent_team)
    {
        case 1:
            this->set_flag_position(2, x,y,z);
            two.flag->held = false;
            break;
        case 2:
            this->set_flag_position(1, x,y,z);
            one.flag->held = false;
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
    if (!f->held)
    {
        r = f->vox->largest_radius();
        STATE::agent_list->objects_within_sphere(f->x, f->y, f->z, r);
        STATE::agent_list->sort_filtered_objects_by_distance();
        for (i=0; i<STATE::agent_list->n_filtered; i++) {
            a = STATE::agent_list->filtered_objects[i];
            if (a==NULL) continue;
            if (!a->status.team || a->status.team == 1) continue;
            // pickup
            if (!a->status.dead)
                if (a->status.pickup_flag())
                    f->held = true;
            break;
        }
    }

    b = one.base;
    r = b->vox->largest_radius();
    STATE::agent_list->objects_within_sphere(b->x, b->y, b->z, r);
    for (i=0; i<STATE::agent_list->n_filtered; i++) {
        a = STATE::agent_list->filtered_objects[i];
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
            a->status.at_base();
        }
    }

    f = two.flag;
    if (!f->held)
    {
        r = f->vox->largest_radius();
        STATE::agent_list->objects_within_sphere(f->x, f->y, f->z, r);
        STATE::agent_list->sort_filtered_objects_by_distance();
        for (i=0; i<STATE::agent_list->n_filtered; i++) {
            a = STATE::agent_list->filtered_objects[i];
            if (a==NULL) continue;
            if (!a->status.team || a->status.team == 2) continue;
            // pickup
            if (!a->status.dead)
                if (a->status.pickup_flag())
                    f->held = true;
            break;
        }
    }

    b = two.base;
    r = b->vox->largest_radius();
    STATE::agent_list->objects_within_sphere(b->x, b->y, b->z, r);
    for (i=0; i<STATE::agent_list->n_filtered; i++) {
        a = STATE::agent_list->filtered_objects[i];
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
            a->status.at_base();
        }
    }
}

bool CTF::remove_agent_from_team(int agent) {
    bool success = false;

    Agent_state* a = ServerState::agent_list->get(agent);
    if (a==NULL) return success;
    
    switch(a->status.team) {
        case 0:
            success = none.remove_agent(agent);
            break;
        case 1:
            success = one.remove_agent(agent);
            break;
        case 2:
            success = two.remove_agent(agent);
            break;
        default:
            break;
    }
    
    return success;
}


#endif

bool CTF::is_at_base(int team, float x, float y, float z)
{
    float d,r;
    switch (team)
    {
        case 1:
            if (one.base->vox == NULL)
                return false;
            d = distancef_squared(x,y,z, one.base->x, one.base->y, one.base->z);
            r = one.base->vox->largest_radius();
            if (d < r*r)
                return true;
            else
                return false;
            break;

        case 2:
            if (two.base->vox == NULL)
                return false;
            d = distancef_squared(x,y,z, two.base->x, two.base->y, two.base->z);
            r = two.base->vox->largest_radius();
            if (d < r*r)
                return true;
            else
                return false;
            break;

        default:
            return false;
    }
    return false;
}


bool CTF::add_agent_to_team(int team, int agent) {
    bool success = false;

    if (team < 0 || team > 2) {
        printf("CTF::add_agent_to_team -- team id %d out of range\n", team);
        return success;
    }

    Agent_state* a = STATE::agent_list->get(agent);
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

    a->status.set_team(team);
    printf("Added agent %d to team %d\n", agent, team);
    return success;
}

void CTF::set_team_color(
    int team,
    unsigned char r, unsigned char g, unsigned char b
)
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
    #if DC_CLIENT
    using ClientState::playerAgent_state;
    if (playerAgent_state.you != NULL)
    {
        if (playerAgent_state.you->status.team == team)
            HudMap::update_team(team);
        Compass::update_team_colors();
    }
    #endif
}

void CTF::get_base_spawn_point(int team, int agent_height, Vec3* spawn)
{
    switch (team)
    {
        case 1:
            one.base->get_spawn_point(agent_height, spawn);
            break;
        case 2:
            two.base->get_spawn_point(agent_height, spawn);
            break;
        case 0:
            spawn = NULL;
            break;
        default:
            printf("CTF::get_base_spawn_point -- invalid team %d\n", team);
            return;
    }
}

void CTF::update_flags_held()
{
    using STATE::agent_list;
    Agent_state* a;
    one.flag->held = false;
    two.flag->held = false;
    for (int i=0; i<agent_list->n_max; i++)
    {
        a = agent_list->a[i];
        if (a == NULL) continue;
        if (a->status.has_flag)
            switch (a->status.team)
            {
                case 1:
                    two.flag->held = true;
                    break;
                case 2:
                    one.flag->held = true;
                    break;
                default:
                    printf("WARNING -- CTF::update_flags_held() -- agent %d has_flag but has team %d\n", a->id, a->status.team);
                    break;
            }
    }
}

void CTF::update()
{
    update_flags_held();
    one.flag->update();
    one.base->update();
    two.flag->update();
    two.base->update();
}

Base* CTF::get_base(int team)
{
    switch (team)
    {
        case 1:
            return one.base;
        case 2:
            return two.base;
        default:
            return NULL;
    }
}

Base* CTF::get_enemy_base(int team)
{
    if (team)
        team = (team == 1) ? 2 : 1;
    return this->get_base(team);
}

Flag* CTF::get_flag(int team)
{
    switch (team)
    {
        case 1:
            return one.flag;
        case 2:
            return two.flag;
        default:
            return NULL;
    }
}

Flag* CTF::get_enemy_flag(int team)
{
    if (team)
        team = (team == 1) ? 2 : 1;
    return this->get_flag(team);
}
