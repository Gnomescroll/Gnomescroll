#pragma once

#include <c_lib/agent/agent.hpp>

#include <c_lib/template/object_list.hpp>
class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }

    public:
        void draw();

        int ids_in_use[AGENT_MAX];

        int objects_within_sphere(float x, float y, float z, float radius);
        void objects_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta);   // origin, direction, cone threshold
        
        void send_to_client(int client_id);

        int get_ids(int* p);
        int get_ids();
};

