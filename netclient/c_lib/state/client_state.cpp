#include "client_state.hpp"
#pragma once

#include <c_lib/agent/agent.hpp>
//#include <c_lib/agent/agent.cpp>
#include <c_lib/agent/net_agent.cpp>

#ifdef DC_CLIENT
namespace ClientState {

    PlayerAgent_state playerAgent_state;

    int active_agent = 0;
    int agent_control_state[16] = {NULL};
        
    void InitClientState() {
    }

    void set_PlayerAgent_id(int id) {
            playerAgent_state.set_PlayerAgent_id(id);
    }

    void set_control_state(int f, int b, int l, int r, int jet, int jump, float theta, float phi) {
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
    void send_control_state() {
    }

    void ClientTick() {
            agent_list.client_tick();
    }
}
#endif
