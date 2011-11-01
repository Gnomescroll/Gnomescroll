#pragma once
#ifdef DC_CLIENT

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/object_lib.hpp>

//#include <c_lib/agent/net_agent.cpp>

namespace ClientState {

    /* SINGLETONS
     * Make sure to extern declare in cython_imports
     *          */
    extern Agent_list agent_list;
    extern Cspray_list cspray_list;
    extern Grenade_list grenade_list;
    extern Shrapnel_list shrapnel_list;
    extern Blood_list blood_list;
    extern Neutron_list neutron_list;
        
    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi);
    void set_PlayerAgent_id(int id);
    
    void send_control_state();
    void InitClientState();

    void ClientTick();
}
#endif
