#include "./wrapper.hpp"

Agent_state* create_agent() {
    Agent_state* a;
    
    #ifdef DC_SERVER
        a = ServerState::agent_list.create();
    #else
        a = ClientState::agent_list.create();
    #endif

    return a;
}

Agent_state* create_agent(int id) {
    Agent_state* a;
    
    #ifdef DC_SERVER
        a = ServerState::agent_list.create(id);
    #else
        a = ClientState::agent_list.create(id);
    #endif

    return a;
}


Agent_state* get_agent(int id) {
    
    Agent_state* a;
    
    #ifdef DC_SERVER
        a = ServerState::agent_list.get(id);
    #else
        a = ClientState::agent_list.get(id);
    #endif

    return a;

}
void delete_agent(int id) {
    #ifdef DC_SERVER
        a = ServerState::agent_list.destroy(id);
    #else
        a = ClientState::agent_list.destroy(id);
    #endif
}