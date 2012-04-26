#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/physics/vec3.hpp>
#include <c_lib/common/template/object_list.hpp>

class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }

        
        //// quicksort helpers
        void quicksort_distance_asc(int beg, int end);
        void quicksort_distance_desc(int beg, int end);
        void swap_object_state(Agent_state **a, Agent_state **b);
        void swap_float(float *a, float *b);
        void quicksort_team(int beg, int end);

        int check_name_interval;
        
    public:
        void update_map_manager_positions();

        void update_models();

        int ids_in_use[AGENT_MAX];
        
        void send_to_client(int client_id);

        int get_ids();

        bool name_available(char* name);

        void check_missing_names();
        void check_if_at_base();
        #if DC_CLIENT
        void update_team_colors();
        #endif

        // filtering
        Agent_state** filtered_objects; // tmp array for filtering objects
        float* filtered_object_distances;
        int n_filtered;

        void filter_none(); // copies pointers/null into filtered list, unchanged
        void sort_filtered_objects_by_distance(bool ascending=true);

        int objects_within_sphere(float x, float y, float z, float radius);
        int enemies_within_sphere(float x, float y, float z, float radius, int team);
        void objects_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta);   // origin, direction, cone threshold
        void sort_by_team();

        Agent_list();
        ~Agent_list()
        {
            if (this->filtered_objects != NULL)
                free(this->filtered_objects);
            if (this->filtered_object_distances != NULL)
                free(this->filtered_object_distances);
        }
};

Agent_state* nearest_agent_in_range(const Vec3 position, const float radius);
Agent_state* random_agent_in_range(const Vec3 position, const float radius);
