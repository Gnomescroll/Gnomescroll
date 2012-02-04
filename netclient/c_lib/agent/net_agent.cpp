#include "net_agent.hpp"

#include <c_lib/agent/agent.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/defines.h>
#include <c_lib/weapons/weapons.hpp>
#include <monsters/monsters.hpp>
#ifdef DC_SERVER
#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/ray_trace.h>
#endif


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

inline void SendClientId_StoC::handle()
{
    //printf("Client Received Client id = %i \n", client_id);
    NetClient::Server.client_id = client_id;
    client_connect_event(client_id);
    if (ClientState::playerAgent_state.you != NULL)
    {
        ClientState::playerAgent_state.you->client_id = client_id;
    }
}


inline void Agent_state_message::handle() {
    Agent_state* A = STATE::agent_list.get(id);
    if(A == NULL) {
        printf("Agent_state_message :: Agent does not exist: create agent, id=%i \n", id);
        return;
    }
    A->handle_state_snapshot(seq, theta, phi, x, y, z, vx, vy, vz);
}

inline void Agent_teleport_message::handle() {
    Agent_state* A = STATE::agent_list.get(id);
    if(A == NULL) {
        printf("Agent_state_message :: Agent does not exist: create agent, id=%i \n", id);
        return;
    }
    // reset camera angle
    if (A->is_you() && current_camera->type == AGENT_CAM)
        current_camera->set_angles(theta, phi);
    A->set_state(x,y,z, vx,vy,vz);
    A->set_angles(theta, phi);
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

inline void agent_shot_object_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.fired_weapon_at_object(target_id, target_type, target_part, x,y,z);
}

inline void agent_shot_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.fired_weapon_at_block(x,y,z, cube, side);
}

inline void agent_shot_nothing_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.fired_weapon_at_nothing();
}

inline void agent_melee_object_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.melee_attack_object(target_id, target_type, target_part, x,y,z);
}

inline void agent_melee_nothing_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.melee_attack_nothing();
}

inline void agent_hit_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.hit_block();
}

inline void agent_threw_grenade_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.threw_grenade();
}

inline void agent_placed_block_StoC::handle()
{
    if (id == ClientState::playerAgent_state.agent_id) return;   // ignore you, should have played locally before transmission
    Agent_state* a = ClientState::agent_list.get(id);
    if (a == NULL) return;
    a->event.placed_block();
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
    if (a==NULL) printf("agent_create_StoC:: get_or_create agent failed\n");
    a->client_id = client_id;
    a->event.joined_team(team);
    //printf("C Agent created. id: %d\n", a->id);
}

inline void agent_name_StoC::handle()
{
    Agent_state* a = ClientState::agent_list.get(id);
    if (a==NULL) printf("agent_name_StoC:: agent %d unknown. Could not name %s\n", id, name);   
    a->status.set_name(name);
    a->event.name_changed();
}

inline void agent_destroy_StoC::handle() {
    ClientState::agent_list.destroy(id);
}

inline void PlayerAgent_id_StoC::handle() {
    ClientState::set_PlayerAgent_id(id);
}

inline void AgentKills_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(id);
    if (a==NULL) return;
    a->status.kills = kills;
}

inline void AgentDeaths_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(id);
    if (a==NULL) return;
    a->status.deaths = deaths;
}

inline void AgentSuicides_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(id);
    if (a==NULL) return;
    a->status.suicides = suicides;
}

inline void AgentActiveWeapon_StoC::handle() {
    Agent_state* a =  ClientState::agent_list.get(id);
    if (a==NULL) return;
    //a->weapons.set_active(slot);  // dont use! will end up in recursive packet chain
    a->weapons.active = slot;
}

inline void AgentReloadWeapon_StoC::handle() {
    Agent_state* a = ClientState::agent_list.get(id);
    if (a==NULL) return;
    a->event.reload_weapon(type);
}

inline void agent_coins_StoC::handle()
{
    Agent_state* a = ClientState::playerAgent_state.you;
    if (a == NULL) return;
    a->event.coins_changed(coins);
}

//inline void identified_StoC::handle(){}
//inline void identify_fail_StoC::handle(){}

inline void Agent_cs_CtoS::handle() {}
inline void hit_block_CtoS::handle() {}
inline void hitscan_object_CtoS::handle() {}
inline void hitscan_block_CtoS::handle() {}
inline void hitscan_none_CtoS::handle() {}
inline void ThrowGrenade_CtoS::handle(){}
inline void AgentActiveWeapon_CtoS::handle() {}
inline void AgentReloadWeapon_CtoS::handle(){}
inline void agent_block_CtoS::handle() {}
inline void place_spawner_CtoS::handle(){}
inline void melee_object_CtoS::handle(){}
inline void melee_none_CtoS::handle(){}
//inline void identify_CtoS::handle(){}
#endif

// Client -> Server handlers
#ifdef DC_SERVER

inline void SendClientId_StoC::handle() {}
inline void AgentKills_StoC::handle() {}
inline void AgentDeaths_StoC::handle() {}
inline void AgentSuicides_StoC::handle() {}
inline void Agent_state_message::handle() {}
inline void Agent_teleport_message::handle() {}
inline void Agent_cs_StoC::handle() {}
inline void agent_damage_StoC::handle() {}
inline void agent_shot_object_StoC::handle(){}
inline void agent_shot_block_StoC::handle(){}
inline void agent_shot_nothing_StoC::handle(){}
inline void agent_melee_object_StoC::handle(){}
inline void agent_melee_nothing_StoC::handle(){}
inline void agent_hit_block_StoC::handle(){}
inline void agent_threw_grenade_StoC::handle(){}
inline void agent_placed_block_StoC::handle(){}
inline void agent_health_StoC::handle() {}
inline void agent_dead_StoC::handle() {}
inline void agent_create_StoC::handle() {}
inline void agent_destroy_StoC::handle() {}
inline void PlayerAgent_id_StoC::handle() {}
inline void AgentActiveWeapon_StoC::handle() {}
inline void AgentReloadWeapon_StoC::handle() {}
inline void agent_name_StoC::handle() {}
inline void agent_coins_StoC::handle() {}
//inline void identified_StoC::handle(){}
//inline void identify_fail_StoC::handle(){}

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

    Agent_state* A = NetServer::agents[client_id];
    if(A == NULL) return;

    //determine if message is new, if so send out
    /*
        Client should send last 2 control states each packet, must handle redundant control state properly
    */
    Agent_cs_StoC M;
    M.id = A->id;
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
    //A->client_id = client_id;

}

// agent hit block action
inline void hit_block_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;

    if (!a->weapons.pick.fire()) return;
    agent_hit_block_StoC msg;
    msg.id = a->id;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
    
    //int dmg = a->weapons.pick.damage;
    int dmg = 32;
    _apply_damage_broadcast(x,y,z, dmg);
}

inline void hitscan_object_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    if (!a->weapons.laser.fire()) return;
    
    void *obj;

    const int agent_dmg = 25;
    const int spawner_dmg = 25;
    int spawner_health;

    float x,y,z;
    switch (type)
    {
        case OBJ_TYPE_AGENT:
            //Agent_state* target = ServerState::agent_list.get(agent_id);
            obj = ServerState::agent_list.get(id);
            if (obj==NULL) return;
            // apply damage
            ((Agent_state*)obj)->status.apply_damage(agent_dmg, a->id, a->type);
            x = ((Agent_state*)obj)->s.x;
            y = ((Agent_state*)obj)->s.y;
            z = ((Agent_state*)obj)->s.z;
            // TODO: Use weapon dmg. Use body_part
            //printf("hitscan agent %d:: %d-%d\n", id, agent_id, body_part);
            break;

        case OBJ_TYPE_SLIME:
            obj = ServerState::slime_list.get(id);
            if (obj==NULL) return;
            // apply damage
            //int dmg = 25;
            //slime->status.apply_damage(dmg, id);
            ServerState::slime_list.destroy(id);
            x = ((Monsters::Slime*)obj)->x;
            y = ((Monsters::Slime*)obj)->y;
            z = ((Monsters::Slime*)obj)->z;
            // TODO: Use weapon dmg. Use body_part
            //printf("hitscan agent %d:: %d-%d\n", id, agent_id, body_part);
            break;

        case OBJ_TYPE_SPAWNER:
            obj = ServerState::spawner_list.get(id);
            if (obj == NULL) return;

            if (((Spawner*)obj)->team == a->status.team
            && ((Spawner*)obj)->owner != a->id)
                return; // teammates cant kill spawners
                
            // apply damage
            spawner_health = ((Spawner*)obj)->take_damage(spawner_dmg);
            if (spawner_health <= 0)
            {
                int coins = ((Spawner*)obj)->get_coins_for_kill(a->status.team);
                a->status.add_coins(coins);
            }
            x = ((Spawner*)obj)->x;
            y = ((Spawner*)obj)->y;
            z = ((Spawner*)obj)->z;
            break;

        case OBJ_TYPE_TURRET:
            printf("hitscan_object_CtoS::handle -- Turrets not implemented\n");
            break;
        default:
            printf("hitscan_object_CtoS::handle -- Unknown object type %d\n", type);
            return;
    }

    agent_shot_object_StoC msg;
    msg.id = a->id;
    msg.target_id = this->id;
    msg.target_type = this->type;
    msg.target_part = this->part;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();

}

// hitscan target:block
inline void hitscan_block_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    //TEMP TODO REMOVE
    a->status.add_coins(100);
    //TEMP TODO REMOVE

    if (!a->weapons.laser.fire()) return;

    // damage block
    int weapon_block_damage = 12;
    _apply_damage_broadcast(x,y,z, weapon_block_damage);
    //printf("hitscan block %d:: %d,%d,%d\n", id, x,y,z);
    // TODO: Use weapon block dmg

    // get collision point on block surface (MOVE THIS TO A BETTER SPOT)
    // send to clients
    int collision[3];
    int pre_collision[3];
    int side[3];
    int cube;
    const float max_l = 500.0f;
    float distance=0.0f;

    float f[3];
    a->s.forward_vector(f);

    float
        fx = a->s.x,
        fy = a->s.y,
        fz = a->s.z + a->camera_height();

    int collided = _ray_cast6(fx,fy,fz, f[0], f[1], f[2], max_l, &distance, collision, pre_collision, &cube, side);
    if (!collided) {
        return;
    }

    // pt of collision
    fx += f[0] * distance;
    fy += f[1] * distance;
    fz += f[2] * distance;

    int cube_side = get_cube_side_from_side_array(side);

    agent_shot_block_StoC msg;
    msg.id = a->id;
    msg.x = fx;
    msg.y = fy;
    msg.z = fz;
    msg.cube = cube;
    msg.side = cube_side;
    msg.broadcast();

}

inline void hitscan_none_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    if (!a->weapons.laser.fire()) return;
    agent_shot_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void melee_object_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    if (!a->weapons.pick.fire()) return;

    const int agent_dmg = 50;
    const int spawner_dmg = 50;
    int spawner_health;
    
    void *obj;
    float x,y,z;
    switch (type)
    {
        case OBJ_TYPE_AGENT:
            obj = ServerState::agent_list.get(id);
            if (obj == NULL) return;
            ((Agent_state*)obj)->status.apply_damage(agent_dmg, a->id, a->type);
            x = ((Agent_state*)obj)->s.x;
            y = ((Agent_state*)obj)->s.y;
            z = ((Agent_state*)obj)->s.z;
            break;

        case OBJ_TYPE_SLIME:
            obj = ServerState::slime_list.get(id);
            if (obj == NULL) return;
            ServerState::slime_list.destroy(id);
            x = ((Monsters::Slime*)obj)->x;
            y = ((Monsters::Slime*)obj)->y;
            z = ((Monsters::Slime*)obj)->z;
            break;
        
        case OBJ_TYPE_SPAWNER:
            obj = ServerState::spawner_list.get(id);
            if (obj==NULL) return;

            if (((Spawner*)obj)->team == a->status.team
            && ((Spawner*)obj)->owner != a->id)
                return; // teammates cant kill spawners
                
            // apply damage
            spawner_health = ((Spawner*)obj)->take_damage(spawner_dmg);
            if (spawner_health <= 0)
            {
                int coins = ((Spawner*)obj)->get_coins_for_kill(a->status.team);
                a->status.add_coins(coins);
            }
            x = ((Spawner*)obj)->x;
            y = ((Spawner*)obj)->y;
            z = ((Spawner*)obj)->z;
            break;
            
        //case OBJ_TYPE_TURRET:
            //obj = ServerState::turret_list.get(id);
            //if (obj==NULL) return;
            //((Turrent*)obj)->take_damage();
            //x = ((Turrent*)obj)->x;
            //y = ((Turrent*)obj)->y;
            //z = ((Turrent*)obj)->z;
            //break;

        default:
            return;
    }

    agent_melee_object_StoC msg;
    msg.id = a->id;
    msg.target_id = this->id;
    msg.target_type = this->type;
    msg.target_part = this->part;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    msg.broadcast();
}

inline void melee_none_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    if (!a->weapons.pick.fire()) return;
    agent_melee_nothing_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

inline void ThrowGrenade_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    if (!a->weapons.grenades.fire()) return;
    agent_threw_grenade_StoC msg;
    msg.id = a->id;
    msg.broadcast();

    static const float grenade_vel = 22.0f; // load from dat later
    vx *= grenade_vel;
    vy *= grenade_vel;
    vz *= grenade_vel;
    //create grenade
    Grenade* g = ServerState::grenade_list.create(x,y,z, vx,vy,vz);
    if (g==NULL) return;
    g->owner = a->id;
}

inline void AgentActiveWeapon_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;
    a->weapons.set_active(slot);
}

inline void AgentReloadWeapon_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;
    a->weapons.reload(type);
    // forward action
    AgentReloadWeapon_StoC msg;
    msg.id = a->id;
    msg.type = type;
    msg.broadcast();
}

inline void agent_block_CtoS::handle() {
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    // fire block applier
    if (!a->weapons.blocks.fire()) return;
    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();
    
    // do block place checks here later
    // problem is, fire/(decrement ammo) packet is separate, and isnt aware of this failure

    // check this player first, most likely to be colliding
    bool collides = false;
    _set(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a))
    {
        collides = true;
    }
    else
    {
        for (int i=0; i<ServerState::agent_list.n_max; i++)
        {
            Agent_state* agent = ServerState::agent_list.a[i];
            if (agent == NULL || agent == a) continue;
            //if (cube_intersects(
                //agent->s.x, agent->s.y, agent->s.z,
                //0.3, 0.3, agent->box.b_height,
                //x,y,z,
                //1, 1, 1))
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    _set(x,y,z,0);  // unset

    if (!collides)
        _set_broadcast(x,y,z, val);    
}

inline void place_spawner_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;
    if (a->status.team == 0) return;
    if (!a->status.can_afford(OBJ_TYPE_SPAWNER)) return;
    if (ServerState::spawner_list.full()) return;
    if (!ServerState::spawner_list.team_spawner_available(a->status.team)) return;
    if (ServerState::spawner_list.point_occupied((int)x, (int)y, (int)z)) return;

    a->status.purchase(OBJ_TYPE_SPAWNER);

    Spawner* s = ServerState::spawner_list.create(x+0.5f,y+0.5f,z);
    if (s==NULL) return;
    s->set_team(a->status.team);
    s->set_owner(a->id);
    s->init_vox();
    Spawner_create_StoC msg;
    s->create_message(&msg);
    msg.broadcast();
}
#endif

/***************************/
#ifdef DC_CLIENT
inline void Spawner_create_StoC::handle()
{
    Spawner* s = ClientState::spawner_list.create(id, x,y,z);
    if (s==NULL)
    {
        printf("WARNING Spawner_create_StoC::handle() -- could not create spawner %d\n", id);
        return;
    }
    s->set_team(team);
    s->set_owner(owner);
    s->init_vox();
}
inline void Spawner_destroy_StoC::handle()
{
    ClientState::spawner_list.destroy(id);
}
#endif

#ifdef DC_SERVER
inline void Spawner_create_StoC::handle() {}
inline void Spawner_destroy_StoC::handle() {}
#endif

//inline void identify_CtoS::handle()
//{
    //Agent_state* a = NetServer::agents[client_id];
    //if (a==NULL)
    //{
        //printf("identify_CtoS : handle -- client_id %d has no agent. could not identify\n", client_id);
        //return;
    //}
    //a->status.set_name(name, a->id);
//}
