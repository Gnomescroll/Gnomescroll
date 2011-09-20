#pragma once

#include <net_lib/client/client.hpp>
#include <net_lib/common/message_handler.h>


class Agent_control_state_message {

    public:
        int id;
        int seq;
        uint32_t cs;
        unsigned int tick;
        unsigned int ctick;
        float theta;
        float phi;

        Agent_control_state_message(); //default constructor
        void register_message();
        void send_message();
        int deserialize(unsigned char* buff, int buff_n);

};

int handle_agent_control_state_message(unsigned char* buff, int buff_n);