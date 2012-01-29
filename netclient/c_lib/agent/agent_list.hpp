#pragma once

#include <c_lib/agent/agent.hpp>

#include <c_lib/template/object_list.hpp>
class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }

        // quicksort helpers
        void quicksort_distance_asc(int beg, int end);
        void quicksort_distance_desc(int beg, int end);
        void swap_agent_state(Agent_state **a, Agent_state **b);
        void swap_float(float *a, float *b);
        
    public:
        void draw();

        int ids_in_use[AGENT_MAX];

        Agent_state* filtered_agents[AGENT_MAX]; // tmp array for filtering agents
        float filtered_agent_distances[AGENT_MAX];
        int n_filtered;
        
        int agents_within_sphere(float x, float y, float z, float radius);
        void agents_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta);   // origin, direction, cone threshold
        Agent_state* hitscan_agents(float x, float y, float z, float vx, float vy, float vz, float pos[3], float* _rad2, float* distance, int ignore_id);
        void sort_filtered_agents_by_distance(bool ascending=true);
        
        void send_to_client(int client_id);

        int get_ids(int* p);
        int get_ids();
};

