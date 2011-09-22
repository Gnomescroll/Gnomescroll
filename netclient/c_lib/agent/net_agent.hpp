#pragma once

#include <net_lib/client/client.hpp>

/*
Agent control state message
*/

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
        int deserialize(unsigned char* buff, int buff_n, int* read_bytes);

};

void handle_agent_control_state_message(unsigned char* buff, int buff_n, int* read_bytes);
void Agent_control_state_message_register_message();

/*
Agent state message
*/


class Agent_state_message {

    public:
        int id;
        int seq;

        unsigned int tick;
        float x,y,x;
        float vx,vy,vz;


        Agent_state_message(); //default constructor
        void register_message();
        void send_message();
        void deserialize(unsigned char* buff, int buff_n, int read_bytes*);

};

void handle_agent_state_message(unsigned char* buff, int buff_n, int* read_bytes);
void Agent_state_message_register_message();