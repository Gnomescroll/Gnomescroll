#pragma once

#include <agent/agent.hpp>
#include <physics/vec3.hpp>
#include <common/template/object_list.hpp>

class Agent_list: public Object_list<Agent_state,AGENT_MAX>
{
    private:
        const char* name() { return "Agent"; }
        
        // quicksort helpers
        void quicksort_distance_asc(int beg, int end);
        void quicksort_distance_desc(int beg, int end);
        void swap_object_state(Agent_state **a, Agent_state **b);
        void swap_float(float *a, float *b);

        int check_name_interval;
        
    public:

    
        #if DC_SERVER
        void update_map_manager_positions();
        void send_to_client(int client_id);

        // temporary object pool, for init phase
        Agent_state** tmp_a;    // tmp pool

        Agent_state* create_temp(int id)
        {
            if (id < 0 || id >= this->n_max) return NULL;
            GS_ASSERT(this->tmp_a[id] == NULL);
            if (this->tmp_a[id] != NULL) return NULL;
            Agent_state* agent = this->create(id);
            if (agent == NULL) return NULL;
            this->tmp_a[id] = agent;
            this->a[id] = NULL;
            return agent;
        }

        void destroy_temp(int id)
        {
            if (id < 0 || id >= this->n_max) return;
            GS_ASSERT(this->tmp_a[id] != NULL);
            if (this->tmp_a[id] != NULL)
            {
                this->a[id] = this->tmp_a[id];
                this->tmp_a[id] = NULL;
            }
            this->destroy(id);
        }

        Agent_state* load_temp(int id)
        {
            if (id < 0 || id >= this->n_max) return NULL;
            GS_ASSERT(this->tmp_a[id] != NULL);
            if (this->tmp_a[id] == NULL) return NULL;
            this->a[id] = this->tmp_a[id];
            this->tmp_a[id] = NULL;
            return this->a[id];
        }

        // will return tmp or main
        Agent_state* get_any(int id)
        {
            if (id < 0 || id >= this->n_max) return NULL;
            Agent_state* agent = this->get(id);
            if (agent == NULL)
                agent = this->tmp_a[id];
            return agent;
        }

        // will destroy tmp or main
        void destroy_any(int id)
        {
            if (id < 0 || id >= this->n_max) return;
            if (this->tmp_a[id] != NULL)
                this->destroy_temp(id);
            else
                this->destroy(id);
        }
        #endif

        void update_models();

        int ids_in_use[AGENT_MAX];
    
        int get_ids();

        // filtering
        Agent_state** filtered_objects; // tmp array for filtering objects
        float* filtered_object_distances;
        int n_filtered;

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

        Agent_list();
        ~Agent_list()
        {
            if (this->filtered_objects != NULL)
                free(this->filtered_objects);
            if (this->filtered_object_distances != NULL)
                free(this->filtered_object_distances);
            #if DC_SERVER
            if (this->tmp_a != NULL)
            {
                for (int i=0; i<this->n_max; i++)
                    if (this->tmp_a[i] != NULL)
                    {
                        GS_ASSERT(this->a[i] != NULL);
                        if (this->a[i] != NULL)
                            delete this->tmp_a[i];
                    }
                free(this->tmp_a);
            }
            #endif
        }
};

Agent_state* nearest_agent_in_range(const Vec3 position, const float radius);
Agent_state* nearest_living_agent_in_range(const Vec3 position, const float radius);
Agent_state* nearest_living_agent_model_in_range(const Vec3 position, const float radius);
Agent_state* random_agent_in_range(const Vec3 position, const float radius);
