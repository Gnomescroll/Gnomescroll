#include "agent_list.hpp"

#include <physics/common.hpp>
#if DC_CLIENT
#include <state/client_state.hpp>
#include <animations/weapon.hpp>
#include <item/properties.hpp>
#include <animations/emitter.hpp>
#endif
#if DC_SERVER
#include <state/server_state.hpp>
#include <t_map/server/manager.hpp>
#endif

const int CHECK_MISSING_NAME_INTERVAL = 30 * 6; // ~ once every 6 seconds

#if DC_SERVER
void Agent_list::update_map_manager_positions()
{
    Vec3 p;
    for(int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->camera_ready)
            p = this->a[i]->camera.get_position();
        else
            p = this->a[i]->get_position();
        t_map::t_map_manager_update_client_position(i, p.x, p.y);
    }
}

void Agent_list::send_to_client(int client_id)
{
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL) continue;
        if (a[i]->client_id == client_id) continue;
        
        agent_create_StoC msg;
        msg.id = a[i]->id;
        msg.client_id = a[i]->client_id;
        strncpy(msg.username, a[i]->status.name, PLAYER_NAME_MAX_LENGTH+1);
        msg.username[PLAYER_NAME_MAX_LENGTH] = '\0';
        msg.sendToClient(client_id);

        if (a[i]->status.color_chosen)
            a[i]->status.send_color(client_id);

        agent_dead_StoC dead_msg;
        dead_msg.dead = a[i]->status.dead;
        dead_msg.id = a[i]->id;
        dead_msg.sendToClient(client_id);

        a[i]->status.send_health_msg(client_id);
        
        int item_type = Toolbelt::get_agent_selected_item_type(i);
        Toolbelt::send_agent_set_active_item_packet(client_id, i, item_type);
    }
}
#endif

#if DC_CLIENT
void Agent_list::draw_names()
{
    int agent_id = ClientState::playerAgent_state.agent_id;
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        Agent_state* a = this->a[i];
        if (a->id == agent_id && current_camera == agent_camera)
            continue;
        a->event.update_hud_name();
        a->event.bb.draw();
    }
}

void Agent_list::draw_equipped_items()
{
    int agent_id = ClientState::playerAgent_state.agent_id;
    if (this->num <= 0) return;

    int ct = 0;
    bool works = Animations::draw_voxel_gl_begin(GL_BACK);
    if (works)
    {
        for (int i=0; i<this->n_max; i++)
        {
            if (this->a[i] == NULL) continue;
            if (this->a[i]->id == agent_id) continue; // skip you
            if (this->a[i]->vox == NULL) continue;
            int equipped_item_type = Toolbelt::get_agent_selected_item_type(i);
            if (!Item::item_type_is_voxel(equipped_item_type)) continue;

            float radius = this->a[i]->vox->get_part(0)->radius;
            Vec3 center = this->a[i]->vox->get_center();
            if (sphere_fulstrum_test_translate(center.x, center.y, center.z, radius) == false)
                continue;

            Animations::draw_equipped_voxel_item_other_agent(i, equipped_item_type);

            ct++;
        }
        Animations::draw_voxel_gl_end();
    }

    GS_ASSERT(this->num-ct >= 0);
    if (this->num - ct <= 0) return;

    works = Animations::draw_sprite_gl_begin();
    if (works)
    {
        for (int i=0; i<this->n_max; i++)
        {
            if (this->a[i] == NULL) continue;
            if (this->a[i]->id == agent_id) continue; // skip you
            if (this->a[i]->vox == NULL) continue;
            int equipped_item_type = Toolbelt::get_agent_selected_item_type(i);
            if (Item::item_type_is_voxel(equipped_item_type)) continue;

            float radius = this->a[i]->vox->get_part(0)->radius;
            Vec3 center = this->a[i]->vox->get_center();
            if (sphere_fulstrum_test_translate(center.x, center.y, center.z, radius) == false)
                continue;

            Animations::draw_equipped_sprite_item_other_agent(i, equipped_item_type);
        }
        Animations::draw_sprite_gl_end();
    }
}

void Agent_list::update_mining_lasers()
{
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL && i != ClientState::playerAgent_state.agent_id)
            this->a[i]->event.update_mining_laser();
}
#endif


/* Agent list */

void Agent_list::update_models() // doesnt actually draw, but updates draw/hitscan properties
{
    for(int i=0; i<n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->update_model();
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

// have to override these because of Agent_state->s.x,y,z
int Agent_list::objects_within_sphere(float x, float y, float z, float radius)
{
    x = translate_point(x);
    y = translate_point(y);
    int ct = 0;
    float dist;
    const float radius_squared = radius*radius;
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL) continue;
        Vec3 p = this->a[i]->get_position();
        p.x = quadrant_translate_f(x, p.x);
        p.y = quadrant_translate_f(y, p.y);
        dist = distancef_squared(x,y,z, p.x, p.y, p.z);
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

// have to override these because of Agent_state->s.x,y,z
int Agent_list::object_models_within_sphere(float x, float y, float z, float radius)
{
    x = translate_point(x);
    y = translate_point(y);
    int ct = 0;
    float dist;
    const float radius_squared = radius*radius;
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (a[i] == NULL || a[i]->vox == NULL) continue;
        Vec3 p = this->a[i]->vox->get_center();
        p.x = quadrant_translate_f(x, p.x);
        p.y = quadrant_translate_f(y, p.y);
        dist = distancef_squared(x,y,z, p.x, p.y, p.z);
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
    x = translate_point(x);
    y = translate_point(y);
    int ct = 0;
    float ax,ay,az;
    float ip;
    float arc;

    float len = sqrtf(vx*vx + vy*vy + vz*vz);
    vx /= len;
    vy /= len;
    vz /= len;
    for (int i=0; i<AGENT_MAX; i++)
    {
        Agent_state* a = this->a[i];
        if (a == NULL) continue;

        Vec3 p = a->get_position();
        p.x = quadrant_translate_f(x, p.x);
        p.y = quadrant_translate_f(y, p.y);
        ax = p.x - x;
        ay = p.y - y;
        az = p.z - z;

        len = sqrtf(ax*ax + ay*ay + az*az);
        ax /= len;
        ay /= len;
        az /= len;

        ip = ax*vx + ay*vy + az*vz;
        arc = abs(acosf(ip));

        if (arc < theta)
            filtered_objects[ct++] = a;
    }

    this->n_filtered = ct;
}

int Agent_list::get_ids()
{
    int j=0;
    for (int i=0; i<AGENT_MAX;i++)
    {
        if (a[i] == NULL) continue;
        if (a[i]->id == 0) continue;// skip 0th agent
        ids_in_use[j] = a[i]->id;
        j++;
    }
    return j;            
}

Agent_list::Agent_list() :
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
    agent_list->sort_filtered_objects_by_distance();
    return agent_list->filtered_objects[0];
}

Agent_state* nearest_living_agent_in_range(const Vec3 position, const float radius)
{
    using STATE::agent_list;
    int n = agent_list->objects_within_sphere(position.x, position.y, position.z, radius);
    if (n <= 0) return NULL;
    agent_list->sort_filtered_objects_by_distance();
    Agent_state* agent = NULL;
    int i=0;
    while (i < n)
    {   // skip all dead agents
        agent = agent_list->filtered_objects[i];
        GS_ASSERT(agent != NULL);
        if (agent != NULL && !agent->status.dead) break;
        i++;
    }
    if (i >= n) agent = NULL;
    return agent;
}

Agent_state* nearest_living_agent_model_in_range(const Vec3 position, const float radius)
{
    using STATE::agent_list;
    int n = agent_list->object_models_within_sphere(position.x, position.y, position.z, radius);
    if (n <= 0) return NULL;
    agent_list->sort_filtered_objects_by_distance();
    Agent_state* agent = NULL;
    int i=0;
    while (i < n)
    {   // skip all dead agents
        agent = agent_list->filtered_objects[i];
        GS_ASSERT(agent != NULL);
        if (agent != NULL && !agent->status.dead) break;
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
    MALLOX(int, chosen, n); //type, name, size

    for (int i=0; i<n; i++)
        chosen[i] = i;
    shuffle_int_array(chosen, n);  // randomize

    // iterate shuffled ids, looking for a non-viewer agent
    Agent_state* agent = NULL;
    int i=0;
    while (i < n)
    {
        agent = agent_list->filtered_objects[chosen[i]];
        i++;
    }
    if (i >= n) agent = NULL;
    return agent;
}
