#include "./wrapper.hpp"

#ifdef DC_SERVER
    #define STATE ServerState
#else
    #define STATE ClientState
#endif


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

Agent_state* get_or_create_agent(int id) {
    Agent_state* a;
    
    a = STATE::agent_list.get(id);
    if (a == NULL) {
        a = STATE::agent_list.create(id);
    }
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
        ServerState::agent_list.destroy(id);
    #else
        ClientState::agent_list.destroy(id);
    #endif
}
