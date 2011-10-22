#include "./wrapper.hpp"

Agent_state* create_agent() {
    Agent_state* a;
    a = agent_list.create();
    return a;
}