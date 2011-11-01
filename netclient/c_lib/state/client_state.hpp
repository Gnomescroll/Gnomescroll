#pragma once

#include <c_lib/agent/agent.hpp>
#include <c_lib/objects/object_lib.hpp>

//#include <c_lib/agent/net_agent.cpp>

#ifdef DC_CLIENT
namespace ClientState {

    /* SINGLETONS
     * Make sure to extern declare in cython_imports
     *          */
    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;
    Shrapnel_list shrapnel_list;
    //Neutron_list neutron_list;
    //Blood_list blood_list;
        
    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi);
    void set_PlayerAgent_id(int id);
    
    void send_control_state();
    void InitClientState();

    void ClientTick();
}
#endif
