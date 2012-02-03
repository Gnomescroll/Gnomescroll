#include "agent_list.hpp"

/* Agent list */

void Agent_list::quicksort_team(int beg, int end)
{
    if (end > beg + 1)
    {
        int team;
        if (this->filtered_objects[beg] == NULL)
            team = 10000;   //arbitrarily high, so it will be placed at the end
        else
            team = this->filtered_objects[beg]->status.team;
        int l = beg, r = end;
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
int Agent_list::objects_within_sphere(float x, float y, float z, float radius) {
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
            filtered_objects[ct] = a[i];
            filtered_object_distances[ct] = dist;
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

void Agent_list::sort_by_team()
{
    this->filter_none();    // copies all non null
    this->quicksort_team(0, this->n_filtered);
}
