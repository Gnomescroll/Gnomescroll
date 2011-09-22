#pragma once



#include <c_lib/agent/agent.hpp>



class Agent_list {
        private:
                int id_c;
        public:
                int num;
                class Agent_state* a[1024];

                Agent_list(); //default constructor
                class Agent_state* get(int id);
                class Agent_state* new_agent();
                void delete_agent(int _id);

                void draw();
                void tick();
        };


