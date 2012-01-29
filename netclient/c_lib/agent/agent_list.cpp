#include "agent_list.hpp"

/* Agent list */

void Agent_list::send_to_client(int client_id) {
    int i;
    for (i=1; i<AGENT_MAX; i++) {   // start at 1, 0-agent shouldnt be sent
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

int Agent_list::agents_within_sphere(float x, float y, float z, float radius) {
    int ct = 0;
    float dist;
    float min_dist = 10000000.0f;
    int closest = -1;
    int i;
    for (i=0; i<AGENT_MAX; i++) {
        if (a[i] == NULL) continue;
        dist = distance(x,y,z, a[i]->s.x, a[i]->s.y, a[i]->s.z);
        if (dist < radius) {
            // agent in sphere
            filtered_agents[ct] = a[i];
            filtered_agent_distances[ct] = dist;
            if (dist < min_dist) {
                min_dist = dist;
                closest = ct;
            }
            ct++;            
        }
    }
    this->n_filtered = ct;
    return closest;
}

// origin, direction, cone threshold
void Agent_list::agents_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta)
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
            filtered_agents[ct++] = a;
    }

    this->n_filtered = ct;
}

void Agent_list::swap_agent_state(Agent_state **a, Agent_state **b)
{Agent_state* t=*a; *a=*b; *b=t;}
void Agent_list::swap_float(float *a, float *b)
{float t=*a; *a=*b; *b=t;}


void Agent_list::quicksort_distance_asc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_agent_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_agent_distances[l] <= dist)
                l++;
            else {
                swap_float(&this->filtered_agent_distances[l], &this->filtered_agent_distances[--r]);
                swap_agent_state(&this->filtered_agents[l], &this->filtered_agents[r]);
            }
        }
        swap_float(&this->filtered_agent_distances[--l], &this->filtered_agent_distances[beg]);
        swap_agent_state(&this->filtered_agents[l], &this->filtered_agents[beg]);
        quicksort_distance_asc(beg, l);
        quicksort_distance_asc(r, end);
    }
}

void Agent_list::quicksort_distance_desc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_agent_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_agent_distances[l] >= dist)
                l++;
            else {
                swap_float(&this->filtered_agent_distances[l], &this->filtered_agent_distances[--r]);
                swap_agent_state(&this->filtered_agents[l], &this->filtered_agents[r]);
            }
        }
        swap_float(&this->filtered_agent_distances[--l], &this->filtered_agent_distances[beg]);
        swap_agent_state(&this->filtered_agents[l], &this->filtered_agents[beg]);
        quicksort_distance_desc(beg, l);
        quicksort_distance_desc(r, end);
    }
}


void Agent_list::sort_filtered_agents_by_distance(bool ascending)
{
    if (ascending) this->quicksort_distance_asc(0, this->n_filtered);
    else this->quicksort_distance_desc(0, this->n_filtered);
}


Agent_state* Agent_list::hitscan_agents(float x, float y, float z, float vx, float vy, float vz, float pos[3], float* _rad2, float* distance, int ignore_id) {
    int i;
    
    float _trad2=0.0f, *trad2=&_trad2;
    float dist;
    float min_dist = 100000.0f; // far away
    Agent_state* agent = NULL;
    float tpos[3];
    for (i=0; i<AGENT_MAX; i++) {
        if (a[i] == NULL) continue;
        if (a[i]->id == ignore_id) continue;
        dist = sphere_line_distance(x,y,z, vx,vy,vz, a[i]->s.x + a[i]->box.box_r, a[i]->s.y + a[i]->box.box_r, a[i]->s.z + 0.5*a[i]->box.b_height, tpos, trad2);
        if (dist < 0.0f || dist > min_dist) continue;
        if (*trad2 > 2.0f) continue;
        min_dist = dist;
        agent = a[i];
        _rad2 = trad2;
        pos = tpos;
    }
    *distance = min_dist;
    return agent;
}

int Agent_list::get_ids(int* p) {
    p = ids_in_use;
    return get_ids();
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

