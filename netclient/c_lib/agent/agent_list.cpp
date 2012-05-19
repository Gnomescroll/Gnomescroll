#include "agent_list.hpp"

#if DC_SERVER
    #include <t_map/server/manager.hpp>
#endif
#include <physics/common.hpp>
#include <state/server_state.hpp>
#include <state/client_state.hpp>

/*
    Warning: using agent position for map loading

*/

const int CHECK_MISSING_NAME_INTERVAL = 30 * 6; // ~ once every 6 seconds

#if DC_SERVER
void Agent_list::update_map_manager_positions()
{
    for(int i=0; i<n_max; i++)
        if (this->a[i] != NULL)
            t_map::t_map_manager_update_client_position(i, this->a[i]->camera.x, this->a[i]->camera.y);
}
#endif

/* Agent list */

void Agent_list::update_models() // doesnt actually draw, but updates draw/hitscan properties
{
    for(int i=0; i<n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->update_model();
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

/* quicksorts */

void Agent_list::swap_object_state(Agent_state **a, Agent_state **b)
{Agent_state* t=*a; *a=*b; *b=t;}

void Agent_list::swap_float(float *a, float *b)
{float t=*a; *a=*b; *b=t;}

void Agent_list::quicksort_distance_asc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_object_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_object_distances[l] <= dist)
                l++;
            else {
                swap_float(&this->filtered_object_distances[l], &this->filtered_object_distances[--r]);
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[r]);
            }
        }
        swap_float(&this->filtered_object_distances[--l], &this->filtered_object_distances[beg]);
        swap_object_state(&this->filtered_objects[l], &this->filtered_objects[beg]);
        quicksort_distance_asc(beg, l);
        quicksort_distance_asc(r, end);
    }
}

void Agent_list::quicksort_distance_desc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_object_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_object_distances[l] >= dist)
                l++;
            else {
                swap_float(&this->filtered_object_distances[l], &this->filtered_object_distances[--r]);
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[r]);
            }
        }
        swap_float(&this->filtered_object_distances[--l], &this->filtered_object_distances[beg]);
        swap_object_state(&this->filtered_objects[l], &this->filtered_objects[beg]);
        quicksort_distance_desc(beg, l);
        quicksort_distance_desc(r, end);
    }
}

void Agent_list::sort_filtered_objects_by_distance(bool ascending)
{
    if (ascending) this->quicksort_distance_asc(0, this->n_filtered);
    else this->quicksort_distance_desc(0, this->n_filtered);
}

void Agent_list::filter_none()
{   // moves all non null objects to the filtered list
    int c = 0;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->filtered_objects[c++] = this->a[i];
    }
    this->n_filtered = c;
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
    const float radius_squared = radius*radius;
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL) continue;
        dist = distancef_squared(x,y,z, a[i]->s.x, a[i]->s.y, a[i]->s.z);
        if (dist < radius_squared)
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
    const float radius_squared = radius*radius;
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL) continue;
        if (a[i]->status.team != enemy_team) continue;
        dist = distancef_squared(x,y,z, a[i]->s.x, a[i]->s.y, a[i]->s.z);
        if (dist < radius_squared)
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

#if DC_CLIENT
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
    this->filtered_objects = (Agent_state**)calloc(this->n_max, sizeof(Agent_state*));
    this->filtered_object_distances = (float*)calloc(this->n_max, sizeof(float));
    print_list((char*)this->name(), this);
}

Agent_state* nearest_agent_in_range(const Vec3 position, const float radius)
{
    using STATE::agent_list;
    int n = agent_list->objects_within_sphere(position.x, position.y, position.z, radius);
    if (n <= 0) return NULL;
    Agent_state* agent = NULL;
    int i=0;
    while (i < n)
    {   // skip all viewing agents
        agent = agent_list->filtered_objects[i];
        GS_ASSERT(agent != NULL);
        if (agent->status.team != NO_TEAM) break;
        i++;
    }
    if (i >= n) agent = NULL;
    return agent;
}

Agent_state* nearest_living_agent_in_range(const Vec3 position, const float radius)
{
    using STATE::agent_list;
    int n = agent_list->objects_within_sphere(position.x, position.y, position.z, radius);
    if (n <= 0) return NULL;
    Agent_state* agent = NULL;
    int i=0;
    while (i < n)
    {   // skip all viewing agents
        agent = agent_list->filtered_objects[i];
        GS_ASSERT(agent != NULL);
        if (!agent->status.dead && agent->status.team != NO_TEAM) break;
        i++;
    }
    if (i >= n) agent = NULL;
    return agent;
}


Agent_state* random_agent_in_range(const Vec3 position, const float radius)
{
    using STATE::agent_list;
    // find nearby players
    int n = agent_list->objects_within_sphere(position.x, position.y, position.z, radius);
    if (n == 0) return NULL;
    
    // target random nearby player
    int chosen[n];
    for (int i=0; i<n; i++)
        chosen[i] = i;
    shuffle_int_array(chosen, n);  // randomize

    // iterate shuffled ids, looking for a non-viewer agent
    Agent_state* agent = NULL;
    int i=0;
    while (i < n)
    {
        agent = agent_list->filtered_objects[chosen[i]];
        if (agent != NULL && agent->status.team != NO_TEAM) break;
        i++;
    }
    if (i >= n) agent = NULL;
    return agent;
}
