#pragma once

#include <c_lib/agent/agent.hpp>

#include <c_lib/template/object_list.hpp>

class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }
        void quicksort_team(int beg, int end);

        int check_name_interval;
        
    public:
        void update_map_manager_positions();

        void update_models();

        int ids_in_use[AGENT_MAX];

        int objects_within_sphere(float x, float y, float z, float radius);
        int enemies_within_sphere(float x, float y, float z, float radius, int team);
        void objects_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta);   // origin, direction, cone threshold
        void sort_by_team();
        
        void send_to_client(int client_id);

        int get_ids();

        bool name_available(char* name);

        void check_missing_names();
        void check_if_at_base();
        #ifdef DC_CLIENT
        void update_team_colors();
        #endif
        
        Agent_list();
};

