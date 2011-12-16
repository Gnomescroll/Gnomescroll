#include "client_state.hpp"

//#ifdef DC_CLIENT


#include <c_lib/agent/agent.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/agent/player_agent.hpp>

#include <c_lib/particles/particle_lib.hpp>
//#include <c_lib/items/items.hpp>

#include <c_lib/game/ctf.hpp>

namespace ClientState {

    Agent_list agent_list;
    Cspray_list cspray_list;
    Grenade_list grenade_list;
    Shrapnel_list shrapnel_list;
    Blood_list blood_list;
    Neutron_list neutron_list;
    Minivox_list minivox_list;
    BillboardText_list billboard_text_list;
    //Items::Flag_list flag_list;
    //Items::Base_list base_list;

    CTF ctf;
    
    PlayerAgent_state playerAgent_state;

    int active_agent = 0;
    int agent_control_state[16];
        
    void InitClientState() {
    }

    void set_PlayerAgent_id(int id) {
        playerAgent_state.set_PlayerAgent_id(id);
    }

    void set_control_state(int f, int b, int l, int r, int jet, int jump, int crouch, int boost, int misc1, int misc2, int misc3, float theta, float phi) {
        /*
                int id;
                int seq;
                uint32_t cs;
                int tick;
                int ctick;
                float theta;
                float phi;
        */

        uint8_t cs = 0;
        if(f) cs |= 1;
        if(b) cs |= 2;
        if(l) cs |= 4;
        if(r) cs |= 8;
        if(jet) cs |= 16;
        if(jump) cs |= 32;
        if(crouch) cs |= 64;
        if(boost) cs |= 128;
        if(misc1) cs |= 256;
        if(misc2) cs |= 512;
        if(misc3) cs |= 1024;
        //printf("cs= %i, f=%i \n", cs, f);

        playerAgent_state.set_control_state(cs, theta, phi);

        return;
    }
        /*
                a_cs.cs = 0;
                if(f) a_cs.cs |= 1;
                if(b) a_cs.cs |= 2;
                if(l) a_cs.cs |= 4;
                if(r) a_cs.cs |= 8;
                if(jet) a_cs.cs |= 16;
                if(jump) a_cs.cs |= 32;

                a_cs.theta = theta;
                a_cs.phi = phi;

                a_cs.seq++;
                a_cs.id = active_agent;
            }
        */
    //void send_control_state() {}

    void ClientTick() {
        //does nothing
        agent_list.client_tick();
    }
}
//#endif
