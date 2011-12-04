#pragma once

#include "net_agent.hpp"

/*
 *  Player Agent Packets
 */

inline void PlayerAgent_Snapshot::handle() {
    #ifdef DC_CLIENT
    ClientState::playerAgent_state.handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
    #endif
    //printf("Received Agent_state_message packet: agent_id= %i \n", id);
    //printf("seq= %i \n", seq);
    return;
}

// Server -> Client handlers
#ifdef DC_CLIENT

inline void Agent_state_message::handle() {
    Agent_state* A = STATE::agent_list.get(id);
    if(A == NULL) {
        printf("Agent_state_message :: Agent does not exist: create agent, id=%i \n", id);
        //A = STATE::agent_list.create(id);
        return;
    }            
    A->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
    return;
}

//Agent control state, server to client
inline void Agent_cs_StoC::handle() {
    Agent_state* A = STATE::agent_list.get(id);
    if(A == NULL) {
        printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
        return;
    }

    //printf("!!! control state= %i \n", cs);
    A->handle_control_state(seq, cs, theta, phi);
    ClientState::playerAgent_state.handle_net_control_state(seq, cs, theta, phi);
}

// damage indicator packet
inline void agent_damage_StoC::handle() {
    Agent_state* a = STATE::agent_list.get(id);
    if(a == NULL) {
        return;
    }
    a->event.took_damage(dmg);
}

// fire weapon action
inline void fire_weapon_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.fired_weapon(weapon_id);
}

inline void agent_dead_StoC::handle() {
    bool _dead = (bool)dead;
    Agent_state* a = ClientState::agent_list.get(id);
    if (a==NULL) return;
    a->event.life_changing(_dead);
}

inline void agent_health_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->status.health = health;
}

inline void agent_create_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get_or_create(id);
    a->owner = owner;
    printf("C Agent created. id: %d\n", a->id);
}

inline void Agent_cs_CtoS::handle() {}
inline void hit_block_CtoS::handle() {}
inline void fire_weapon_CtoS::handle() {}
inline void hitscan_agent_CtoS::handle() {}
inline void hitscan_block_CtoS::handle() {}

#endif


// Client -> Server handlers
#ifdef DC_SERVER

inline void Agent_state_message::handle() {}
inline void Agent_cs_StoC::handle() {}
inline void agent_damage_StoC::handle() {}
inline void fire_weapon_StoC::handle() {}
inline void agent_health_StoC::handle() {}
inline void agent_dead_StoC::handle() {}
inline void agent_create_StoC::handle() {}

//for benchmarking
//static int _total = 0;
//static const int a_DEBUG = 1;

inline void Agent_cs_CtoS::handle() {
    //printf("cs_CtoS: seq= %i \n", seq);

    //for benchmarking
    /*
    if(a_DEBUG)
    {
        _total++;
        if(_total % 1000 == 0) printf("%i messages\n", _total);
        return;
    }
    */

    Agent_state* A = STATE::agent_list.get(id);
    if(A == NULL) {
        //STATE::agent_list.create(id);
        printf("Agent_control_to_client_message: agent does not exist, id= %i\n", id);
        return;
    }


    //determine if message is new, if so send out
    /*
        Client should send last 2 control states each packet, must handle redundant control state properly
    */
    Agent_cs_StoC M;
    M.id = id;
    M.seq = seq;
    M.cs = cs;
    M.theta = theta;
    M.phi = phi;
    M.broadcast();

    A->handle_control_state(seq, cs, theta, phi);

    /*
        Warning: setting agent client id by the client the last control state was received for that agent
        This needs to be done properly, at agent creation
    */
    A->client_id = client_id;

}

// agent hit block action
inline void hit_block_CtoS::handle() {
    Agent_state* a = STATE::agent_list.get(id);
    if(a == NULL) {
        return;
    }
    // check that agent has this weapon equipped
    //if (! Weapons::isBlockPick(a->active_weapon.type)) {
        //return;
    //}
    //int dmg = a->active_weapon.damage;
    int dmg = 32;
    // apply damage to block & broadcast
    _apply_damage_broadcast(x,y,z, dmg);
}

// fire weapon action
inline void fire_weapon_CtoS::handle() {
    // forward the packet
    fire_weapon_StoC* msg = new fire_weapon_StoC(id, weapon_id);
    msg->broadcast();
}

// hitscan target:agent
inline void hitscan_agent_CtoS::handle() {
    Agent_state* a = ServerState::agent_list.get(id);
    if (a==NULL) return;
    Agent_state* target = ServerState::agent_list.get(agent_id);
    if (target==NULL) return;
    // apply damage
    int dmg = 25;
    target->status.apply_damage(dmg, id);
    // TODO: Use weapon dmg. Use body_part
    printf("hitscan agent %d:: %d-%d\n", id, agent_id, body_part);
}

// hitscan target:block
inline void hitscan_block_CtoS::handle() {
    Agent_state* a = ServerState::agent_list.get(id);
    if (a==NULL) return;
    // shoot block
    int weapon_block_damage = 12;
    _apply_damage_broadcast(x,y,z, weapon_block_damage);
    printf("hitscan block %d:: %d,%d,%d\n", id, x,y,z);
    // TODO: Use weapon block dmg
}

#endif
