#include "agent_list.hpp"

#include <physics/common.hpp>
#include <agent/_state.hpp>
#include <state/state.hpp>

#if DC_CLIENT
# include <animations/weapon.hpp>
# include <item/properties.hpp>
# include <animations/emitter.hpp>
#endif
#if DC_SERVER
# include <t_map/server/manager.hpp>
# include <social/badges.hpp>
#endif

namespace Agents
{

const int CHECK_MISSING_NAME_INTERVAL = 30 * 6; // ~ once every 6 seconds

#if DC_SERVER
void AgentList::update_map_manager_positions()
{
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        Vec3 p = this->objects[i].camera.get_position();
        if (!this->objects[i].camera_ready)
            p = this->objects[i].get_position();
        t_map::t_map_manager_update_client_position(this->objects[i].client_id, p.x, p.y);
    }
}

void AgentList::send_to_client(ClientID client_id)
{
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        if (this->objects[i].client_id == client_id) continue;

        class Agent* agent = &this->objects[i];

        agent_create_StoC msg;
        msg.id = agent->id;
        msg.client_id = agent->client_id;
        strncpy(msg.username, agent->status.name, PLAYER_NAME_MAX_LENGTH+1);
        msg.username[PLAYER_NAME_MAX_LENGTH] = '\0';
        msg.color = agent->status.color;
        msg.sendToClient(client_id);

        agent_dead_StoC dead_msg;
        dead_msg.dead = agent->status.dead;
        dead_msg.id = agent->id;
        dead_msg.sendToClient(client_id);

        ItemType item_type = Toolbelt::get_agent_selected_item_type(agent->id);
        Toolbelt::send_agent_set_active_item_packet(client_id, agent->id, item_type);

        for (size_t j=0; j<agent->status.n_badges; j++)
            Badges::send_badge(agent->status.badges[j], agent->id, client_id);
    }
}

void AgentList::tick_hunger()
{
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        Agent* agent = &this->objects[i];
        agent->status.tick_hunger();
    }
}

void AgentList::tick_attributes()
{
    static int ac = 0;
    ac++;

    int t0 = _GET_MS_TIME();

    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        Agent* agent = &this->objects[i];

        if(ac % 8 == 0) //4 times a second
            agent->status.tick_rad();
    }

    int t1 = _GET_MS_TIME();

    if(t1-t0 > 2)
        printf("WARNING: AgentList::tick_attributes took %d ms\n", t1-t0);
}

#endif

#if DC_CLIENT
void AgentList::draw_names()
{
    AgentID agent_id = ClientState::player_agent.agent_id;
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        Agent* a = &this->objects[i];
        if (a->id == agent_id && current_camera == agent_camera)
            continue;
        a->event.update_hud_name();
        a->event.bb.draw();
    }
}

void AgentList::draw_badges()
{
    AgentID agent_id = ClientState::player_agent.agent_id;
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        Agent* a = &this->objects[i];
        if (a->id == agent_id && current_camera == agent_camera)
            continue;
        a->event.draw_badges();
    }
}

void AgentList::draw_equipped_items()
{
    if (this->ct <= 0) return;
    const AgentID agent_id = ClientState::player_agent.agent_id;

    unsigned int num = 0;
    bool works = Animations::draw_voxel_gl_begin(GL_BACK);
    if (works)
    {
        for (size_t i=0; i<this->max; i++)
        {
            if (this->objects[i].id == this->null_id) continue;
            if (this->objects[i].id == agent_id) continue; // skip you
            if (this->objects[i].vox == NULL) continue;
            ItemType equipped_item_type = Toolbelt::get_agent_selected_item_type(this->objects[i].id);
            if (!Item::item_type_is_voxel(equipped_item_type)) continue;

            float radius = this->objects[i].vox->get_part(0)->radius / 3;
            Vec3 center = this->objects[i].vox->get_center();
            if (!sphere_fulstrum_test_translate(center.x, center.y, center.z, radius))
                continue;

            Animations::draw_equipped_voxel_item_other_agent(this->objects[i].id, equipped_item_type);

            num++;
        }
        Animations::draw_voxel_gl_end();
    }

    IF_ASSERT(this->ct < num) return;
    if (this->ct == num) return;

    if (Options::animation_level <= 1)
        works = Animations::draw_sprite_gl_begin();
    else
        works = Animations::draw_voxelized_sprite_gl_begin(GL_FRONT);
    if (works)
    {
        for (size_t i=0; i<this->max; i++)
        {
            if (this->objects[i].id == this->null_id) continue;
            if (this->objects[i].id == agent_id) continue; // skip you
            if (this->objects[i].vox == NULL) continue;
            ItemType equipped_item_type = Toolbelt::get_agent_selected_item_type(this->objects[i].id);
            if (Item::item_type_is_voxel(equipped_item_type)) continue;

            float radius = this->objects[i].vox->get_part(0)->radius;
            Vec3 center = this->objects[i].vox->get_center();
            if (!sphere_fulstrum_test_translate(center.x, center.y, center.z, radius))
                continue;

            if (Options::animation_level <= 1)
                Animations::draw_equipped_sprite_item_other_agent(this->objects[i].id, equipped_item_type);
            else
                Animations::draw_equipped_voxelized_sprite_item_other_agent(this->objects[i].id, equipped_item_type);
        }
        if (Options::animation_level <= 1)
            Animations::draw_sprite_gl_end();
        else
            Animations::draw_voxelized_sprite_gl_end();
    }
}

void AgentList::update_mining_lasers()
{
    for (size_t i=0; i<this->max; i++)
        if (this->objects[i].id != this->null_id && i != ClientState::player_agent.agent_id)
            this->objects[i].event.update_mining_laser();
}
#endif


/* Agent list */

void AgentList::update_models() // doesnt actually draw, but updates draw/hitscan properties
{
    for (size_t i=0; i<this->max; i++)
        if (this->objects[i].id != this->null_id)
            this->objects[i].update_model();
}

/* quicksorts */

void AgentList::swap_object_state(Agent** a, Agent** b)
{
    Agent* t = *a;
    *a = *b;
    *b = t;
}

void AgentList::swap_float(float* a, float* b)
{
    float t = *a;
    *a = *b;
    *b = t;
}

void AgentList::quicksort_distance_asc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_object_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_object_distances[l] <= dist)
                l++;
            else
            {
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

void AgentList::quicksort_distance_desc(int beg, int end)
{
    if (end > beg + 1)
    {
        float dist = this->filtered_object_distances[beg];
        int l = beg + 1, r = end;
        while (l < r)
        {
            if (this->filtered_object_distances[l] >= dist)
                l++;
            else
            {
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

void AgentList::sort_filtered_objects_by_distance(bool ascending)
{
    if (ascending)
        this->quicksort_distance_asc(0, this->n_filtered);
    else
        this->quicksort_distance_desc(0, this->n_filtered);
}

void AgentList::filter_none()
{   // moves all non null objects to the filtered list
    unsigned int c = 0;
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        this->filtered_objects[c++] = &this->objects[i];
    }
    this->n_filtered = c;
}

// have to override these because of Agent->s.x,y,z
int AgentList::objects_within_sphere(const Vec3& position, float radius)
{
    const Vec3 p = translate_position(position);
    unsigned int num = 0;
    const float radius_squared = radius * radius;
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id) continue;
        Vec3 q = this->objects[i].get_position();
        q = quadrant_translate_position(p, q);
        float dist = vec3_distance_squared(p, q);
        if (dist < radius_squared)
        {   // agent in sphere
            this->filtered_objects[num] = &this->objects[i];
            this->filtered_object_distances[num] = dist;
            num++;
        }
    }
    this->n_filtered = num;
    return num;
}

// have to override these because of Agent->s.x,y,z
int AgentList::object_models_within_sphere(const Vec3& position, float radius)
{
    const Vec3 p = translate_position(position);
    unsigned int num = 0;
    const float radius_squared = radius*radius;
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id || this->objects[i].vox == NULL) continue;
        Vec3 q = this->objects[i].vox->get_center();
        q = quadrant_translate_position(p, q);
        float dist = vec3_distance_squared(p, q);
        if (dist < radius_squared)
        {   // agent in sphere
            this->filtered_objects[num] = &this->objects[i];
            this->filtered_object_distances[num] = dist;
            num++;
        }
    }
    this->n_filtered = num;
    return num;
}

// origin, direction, cone threshold
void AgentList::objects_in_cone(const Vec3& position, const Vec3& direction, float theta)
{
    Vec3 p = translate_position(position);
    Vec3 v = vec3_normalize(direction);
    unsigned int num = 0;
    for (size_t i=0; i<this->max; i++)
    {
        Agent* a = &this->objects[i];
        if (a->id == this->null_id) continue;

        Vec3 q = quadrant_translate_position(p, a->get_position());
        Vec3 at = vec3_normalize(vec3_sub(q, p));
        float ip = vec3_dot(at, v);
        float arc = abs(acosf(ip));
        if (arc < theta)
            filtered_objects[num++] = a;
    }
    this->n_filtered = num;
}

Agent* nearest_agent_in_range(const Vec3& position, float radius)
{
    using Agents::agent_list;
    int n = agent_list->objects_within_sphere(position, radius);
    if (n <= 0) return NULL;
    agent_list->sort_filtered_objects_by_distance();
    return agent_list->filtered_objects[0];
}

Agent* nearest_living_agent_in_range(const Vec3& position, float radius)
{
    using Agents::agent_list;
    int n = agent_list->objects_within_sphere(position, radius);
    if (n <= 0) return NULL;
    agent_list->sort_filtered_objects_by_distance();
    Agent* agent = NULL;
    int i = 0;
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

Agent* nearest_living_agent_model_in_range(const Vec3& position, float radius)
{
    using Agents::agent_list;
    int n = agent_list->object_models_within_sphere(position, radius);
    if (n <= 0) return NULL;
    agent_list->sort_filtered_objects_by_distance();
    Agent* agent = NULL;
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

Agent* random_agent_in_range(const Vec3& position, float radius)
{
    using Agents::agent_list;
    // find nearby players
    int n = agent_list->objects_within_sphere(position, radius);
    if (n == 0) return NULL;

    // target random nearby player
    MALLOX(int, chosen, n); //type, name, size

    for (int i=0; i<n; i++) chosen[i] = i;
    shuffle<int>(chosen, n);  // randomize

    // iterate shuffled ids, looking for a non-viewer agent
    Agent* agent = NULL;
    int i=0;
    while (i < n)
    {
        agent = agent_list->filtered_objects[chosen[i]];
        i++;
    }
    if (i >= n) agent = NULL;
    return agent;
}

AgentList::AgentList(size_t capacity) :
    ObjectList<Agent, AgentID>(capacity, NULL_AGENT)
{
    this->filtered_objects = (Agent**)calloc(this->max, sizeof(Agent*));
    this->filtered_object_distances = (float*)calloc(this->max, sizeof(float));
}

}   // Agents
