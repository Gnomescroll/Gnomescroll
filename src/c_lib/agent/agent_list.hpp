/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <agent/agent.hpp>
#include <physics/vec3.hpp>
#include <common/template/object_list.hpp>

namespace Agents
{

class AgentList: public ObjectList<class Agent, AgentID>
{
    private:

    const char* name()
    {
        return "Agent";
    }

    // quicksort helpers
    void quicksort_distance_asc(int beg, int end);
    void quicksort_distance_desc(int beg, int end);
    void swap_object_state(Agent** a, Agent** b);
    void swap_float(float* a, float* b);

    public:
        Agent** filtered_objects; // tmp array for filtering objects
        float* filtered_object_distances;
        unsigned int n_filtered;

    void update_models();

    void filter_none(); // copies pointers/null into filtered list, unchanged
    void sort_filtered_objects_by_distance(bool ascending=true);
    int objects_within_sphere(const Vec3& position, float radius);
    int object_models_within_sphere(const Vec3& position, float radius);
    void objects_in_cone(const Vec3& position, const Vec3& direction, float theta);   // origin, direction, cone threshold

    #if DC_CLIENT
    void draw_names();
    void draw_badges();
    void draw_equipped_items();
    void update_mining_lasers();
    #endif

    #if DC_SERVER
    void update_map_manager_positions();
    void send_to_client(ClientID client_id);
    void tick_hunger();
    void tick_attributes();

    #endif

    explicit AgentList(size_t capacity);

    ~AgentList()
    {
        free(this->filtered_objects);
        free(this->filtered_object_distances);
    }
};

Agent* nearest_agent_in_range(const Vec3& position, float radius);
Agent* nearest_living_agent_in_range(const Vec3& position, float radius);
Agent* nearest_living_agent_model_in_range(const Vec3& position, float radius);
Agent* random_agent_in_range(const Vec3& position, float radius);

}   // Agents
