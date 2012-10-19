#pragma once

#include <agent/agent.hpp>
#include <physics/vec3.hpp>
#include <common/template/object_list.hpp>

#if DC_SERVER
class AgentListTemp: public ObjectList<Agent, AgentID>
{
    private:
        const char* name() { return "AgentListTemp"; }
    public:
        AgentListTemp(unsigned int capacity) :
            ObjectList<Agent, AgentID>(capacity, NULL_AGENT)
        {
            this->print();
        }

        class Agent* create(AgentID id)
        {
            class Agent* agent = ObjectList<Agent, AgentID>::create(id);
            if (agent != NULL) agent->temp = true;
            return agent;
        }
};
#endif

class AgentList: public ObjectList<class Agent, AgentID>
{
    private:
        const char* name() { return "Agent"; }
        
        // quicksort helpers
        void quicksort_distance_asc(int beg, int end);
        void quicksort_distance_desc(int beg, int end);
        void swap_object_state(Agent **a, Agent **b);
        void swap_float(float *a, float *b);

        int check_name_interval;
        
    public:

    
        #if DC_SERVER
        void update_map_manager_positions();
        void send_to_client(ClientID client_id);

        // load existing agent into this one
        // used for transferring tmp agent to this one
        class Agent* load(class Agent* agent);
        #endif

        void update_models();

        int ids_in_use[MAX_AGENTS];
    
        int get_ids();

        // filtering
        Agent** filtered_objects; // tmp array for filtering objects
        float* filtered_object_distances;
        unsigned int n_filtered;

        void filter_none(); // copies pointers/null into filtered list, unchanged
        void sort_filtered_objects_by_distance(bool ascending=true);

        int objects_within_sphere(float x, float y, float z, float radius);
        int object_models_within_sphere(float x, float y, float z, float radius);
        void objects_in_cone(float x, float y, float z, float vx, float vy, float vz, float theta);   // origin, direction, cone threshold

        #if DC_CLIENT
        void draw_names();
        void draw_equipped_items();
        void update_mining_lasers();
        #endif

        explicit AgentList(unsigned int capacity);
        
        ~AgentList()
        {
            if (this->filtered_objects != NULL)
                free(this->filtered_objects);
            if (this->filtered_object_distances != NULL)
                free(this->filtered_object_distances);
        }
};

Agent* nearest_agent_in_range(const Vec3 position, const float radius);
Agent* nearest_living_agent_in_range(const Vec3 position, const float radius);
Agent* nearest_living_agent_model_in_range(const Vec3 position, const float radius);
Agent* random_agent_in_range(const Vec3 position, const float radius);
