#pragma once

#include <c_lib/agent/agent.hpp>

#ifdef DC_CLIENT
        #include <c_lib/agent/agent_draw.hpp>
#endif


class Agent_list {
	private:
                int id_c;
        public:
                int num
                Agent_state* a[1024];

                Agent_list(); //default constructor
                Agent_state* get(int id);
                Agent_state* new_agent();
                void delete_agent();

                void draw();
                void tick();
        };

