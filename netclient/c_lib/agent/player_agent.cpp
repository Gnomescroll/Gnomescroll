
#pragma once
/*
#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>
#include <c_lib/agent/agent.hpp>
*/

#include <c_lib/agent/net_agent.hpp>

//#include <net_lib/server/server.h>

//#include <c_lib/template/net.hpp>

//#include <net_lib/client/client.hpp>

//#include <c_lib/agent/net_agent.cpp>

/*
    Use for client side and state update to server

*/

class PlayerAgent_state {
    public:
        int agent_id;
        int seq;
        
        //last agent state update

        Agent_cs_CtoS cs_0; //last control state
        //Agent_cs_CtoS cs_1;

    PlayerAgent_state() {
        agent_id = -1;
        seq = 0;
    }

    void set_PlayerAgent_id(int id) {
        agent_id = id;
    }

    //set actually sends
    void set_control_state(uint16_t cs, float theta, float phi) {
        if(agent_id == -1) return;  //player agent not set

        seq = (seq+1) % 256;

        Agent_cs_CtoS csp;

        csp.id = agent_id;
        csp.seq = seq;
        csp.cs = cs;
        csp.theta = theta;
        csp.phi = phi;

        csp.send();

        //printf("control state send: seq= %i, cs= %i \n", seq, cs);
    }


};
