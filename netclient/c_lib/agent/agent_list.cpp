#include "agent_list.hpp"

#ifdef DC_SERVER
    #include <c_lib/t_map/server/manager.hpp>
#endif
#include <c_lib/physics/common.hpp>

/*
    Warning: using agent position for map loading

*/

void Agent_list::update_map_manager_positions()
{
#ifdef DC_SERVER
    Agent_state* agent;
    for(int i=0; i<n_max; i++)
    {
        agent = a[i];
        if (agent == NULL) continue;
        t_map::t_map_manager_update_client_position(i, agent->s.x, agent->s.y);
    }
#endif
}

/* Agent list */

void Agent_list::update_models() // doesnt actually draw, but updates draw/hitscan properties
{
    #if DC_CLIENT
    bool you;
    Agent_state* agent;
    for(int i=0; i<n_max; i++)
    {
        agent = a[i];
        if (agent == NULL) continue;
        you = (agent->id == ClientState::playerAgent_state.agent_id);
        if (agent->vox == NULL) continue;
        agent->vox->was_updated = false;

        if (current_camera->first_person && you)
        {   // your agent
            agent->vox->set_draw(false);
            agent->vox->set_hitscan(false);
            continue;
        }

        // remaining agents
        VoxDat* vox_dat = &agent_vox_dat;
        if (current_camera == NULL || !current_camera->in_view(agent->s.x, agent->s.y, agent->s.z))
        {   // agent not in view fulcrum
            agent->vox->set_draw(false);
            agent->vox->set_hitscan(false);
            if (agent->event.bb != NULL)
                agent->event.bb->set_draw(false);
            continue;
        }
        if (agent->event.bb != NULL)
            agent->event.bb->set_draw(true);
        if (agent->crouched())
        {
            vox_dat = &agent_vox_dat_crouched;
            if (!agent->status.vox_crouched)
            {
                agent->vox->set_vox_dat(vox_dat);
                agent->vox->reset_skeleton();
                agent->status.vox_crouched = true;
            }
        }
        else
        {
            if (agent->status.vox_crouched)
            {   // was crouched last frame, but not this frame: restore standing model
                agent->vox->set_vox_dat(vox_dat);
                agent->vox->reset_skeleton();
                agent->status.vox_crouched = false;
            }
        }
        if (agent->status.dead)
            vox_dat = &agent_vox_dat_dead;
            
        agent->vox->set_vox_dat(vox_dat);
        agent->vox->update(agent->s.x, agent->s.y, agent->s.z, agent->s.theta, agent->s.phi);
        agent->vox->set_draw(true);
        agent->vox->set_hitscan(true);
    }
    #endif
    
    #if DC_SERVER
    Agent_state* agent;
    VoxDat* vox_dat;
    for(int i=0; i<n_max; i++)
    {
        agent = a[i];
        if (agent == NULL) continue;
        if (agent->vox == NULL) continue;
        agent->vox->was_updated = false;
        
        vox_dat = &agent_vox_dat;
        if (agent->crouched())
        {
            vox_dat = &agent_vox_dat_crouched;
            if (!agent->status.vox_crouched)
            {
                agent->vox->set_vox_dat(vox_dat);
                agent->vox->reset_skeleton();
                agent->status.vox_crouched = true;
            }
        }
        else
        {
            if (agent->status.vox_crouched)
            {   // was crouched last frame, but not this frame: restore standing model
                agent->vox->set_vox_dat(vox_dat);
                agent->vox->reset_skeleton();
                agent->status.vox_crouched = false;
            }
        }
        if (agent->status.dead)
            vox_dat = &agent_vox_dat_dead;

        agent->vox->set_vox_dat(vox_dat);
        agent->vox->update(agent->s.x, agent->s.y, agent->s.z, agent->s.theta, agent->s.phi);
        agent->vox->set_hitscan(true);
    }
    #endif
}

void Agent_list::quicksort_team(int beg, int end)
{
    if (end > beg + 1)
    {
        int team;
        if (this->filtered_objects[beg] == NULL)
            team = 10000;   //arbitrarily high, so it will be placed at the end
        else
            team = this->filtered_objects[beg]->status.team;
        int l = beg + 1, r = end;
        while (l < r)
        {
            int t;
            t = (this->filtered_objects[l] == NULL) ? 10000 : this->filtered_objects[l]->status.team;
            if (t <= team)
                l++;
            else
            {
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[--r]);
            }
        }
        swap_object_state(&this->filtered_objects[--l], &this->filtered_objects[beg]);
        quicksort_team(beg, l);
        quicksort_team(r, end);
    }
}

void Agent_list::send_to_client(int client_id) {
    int i;
    for (i=0; i<AGENT_MAX; i++) {
        if (a[i]==NULL) continue;
        agent_create_StoC msg;
        msg.id = a[i]->id;
        msg.team = a[i]->status.team;
        msg.client_id = a[i]->client_id;
        msg.sendToClient(client_id);

        agent_name_StoC name_msg;
        name_msg.id = a[i]->id;
        strcpy(name_msg.name, a[i]->status.name);
        name_msg.sendToClient(client_id);
    }
}

// have to override these because of Agent_state->s.x,y,z
int Agent_list::objects_within_sphere(float x, float y, float z, float radius)
{
    int ct = 0;
    float dist;
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL) continue;
        dist = distancef(x,y,z, a[i]->s.x, a[i]->s.y, a[i]->s.z);
        if (dist < radius)
        {   // agent in sphere
            filtered_objects[ct] = a[i];
            filtered_object_distances[ct] = dist;
            ct++;            
        }
    }
    this->n_filtered = ct;
    return ct;
}

int Agent_list::enemies_within_sphere(float x, float y, float z, float radius, int team)
{
    if (!team) return 0;
    int enemy_team = (team == 1) ? 2 : 1; // swap to enemy team id
    int ct = 0;
    float dist;
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL) continue;
        if (a[i]->status.team != enemy_team) continue;
        dist = distancef(x,y,z, a[i]->s.x, a[i]->s.y, a[i]->s.z);
        if (dist < radius)
        {   // agent in sphere
            filtered_objects[ct] = a[i];
            filtered_object_distances[ct] = dist;
            ct++;            
        }
    }
    this->n_filtered = ct;
    return ct;
}

// origin, direction, cone threshold
void Agent_list::objects_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta)
{
    int ct = 0;
    float ax,ay,az;
    float ip;
    float arc;

    float len = sqrt(vx*vx + vy*vy + vz*vz);
    vx /= len;
    vy /= len;
    vz /= len;
    for (int i=0; i<AGENT_MAX; i++)
    {
        Agent_state* a = this->a[i];
        if (a == NULL) continue;

        ax = a->s.x - x;
        ay = a->s.y - y;
        az = a->s.z - z;

        len = sqrt(ax*ax + ay*ay + az*az);
        ax /= len;
        ay /= len;
        az /= len;

        ip = ax*vx + ay*vy + az*vz;
        arc = abs(acos(ip));

        if (arc < theta)
            filtered_objects[ct++] = a;
    }

    this->n_filtered = ct;
}

int Agent_list::get_ids() {
    int i,j=0;
    for (i=0; i<AGENT_MAX;i++) {
        if (a[i] == NULL) continue;
        if (a[i]->id == 0) continue;// skip 0th agent
        ids_in_use[j] = a[i]->id;
        j++;
    }
    return j;            
}

void Agent_list::sort_by_team()
{
    this->filter_none();    // copies all non null
    this->quicksort_team(0, this->n_filtered);
}

bool Agent_list::name_available(char* name)
{
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->status.name == NULL) continue;
        if (!this->a[i]->status.identified) continue;
        if (!strcmp(this->a[i]->status.name, name)) return false;
    }
    return true;
}

void Agent_list::check_missing_names()
{
    this->check_name_interval++;
    if ((this->check_name_interval %= CHECK_MISSING_NAME_INTERVAL) != 0)
        return;

    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->status.check_missing_name();
    }
}

void Agent_list::check_if_at_base()
{
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->status.check_if_at_base();
    }
}

#ifdef DC_CLIENT
void Agent_list::update_team_colors()
{
    if (ClientState::ctf == NULL) return;
    unsigned char r1,g1,b1;
    unsigned char r2,g2,b2;
    ClientState::ctf->get_team_color(1, &r1, &g1, &b1);
    ClientState::ctf->get_team_color(2, &r2, &g2, &b2);
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->status.team == 0) continue;
        switch (this->a[i]->status.team)
        {
            case 1:
                this->a[i]->event.update_team_color(r1, g1, b1);
                break;
            case 2:
                this->a[i]->event.update_team_color(r2, g2, b2);
                break;
            default: continue;
        }
    }
}
#endif

Agent_list::Agent_list()
:
check_name_interval(0)
{
    print();
}

