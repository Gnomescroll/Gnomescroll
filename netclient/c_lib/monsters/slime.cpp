#include "monsters.hpp"

#include <math.h>

#include <c_lib/objects/common/api/born.hpp>
#include <c_lib/objects/common/api/update.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/common/random.h>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

void slimeDropItem(Vec3 position);

VoxDat slime_vox_dat;

void Slime::load_data(int id)
{
    this->_state.id = id;

    this->_state.coin_rule = COINS_ANYONE;

    this->_state.explosion_radius = SLIME_EXPLOSION_RADIUS;
    this->_state.explosion_damage = SLIME_EXPLOSION_DAMAGE;
    this->_state.explosion_proximity_radius = SLIME_EXPLOSION_PROXIMITY_RADIUS;

    this->_state.motion_proximity_radius = SLIME_MOTION_PROXIMITY_RADIUS;

    this->voxel_properties.init_hitscan = true;
    this->voxel_properties.init_draw = true;
    this->voxel_properties.vox_dat = &slime_vox_dat;

    this->spatial_properties.camera_height = SLIME_CAMERA_HEIGHT;
    this->spatial_properties.height = SLIME_HEIGHT;

    this->health_properties.health = SLIME_HEALTH;

    // TODO -- make this a base property
    // momentum should not be used this way (can be overwriiten, is only init etc)
    this->set_momentum(SLIME_SPEED, SLIME_SPEED, SLIME_SPEED);
}

Slime::Slime(int id)
#ifdef DC_SERVER
: tick_num(0), network_state_update_interval(30)
#endif
{
    this->load_data(id);
}

void Slime::born()
{
    ObjectState* state = this->state();
    this->voxel_properties.vox = bornVox(this->voxel_properties.vox_dat, state->id, state->type);
    bornSetVox(
        this->voxel_properties.vox,
        this->voxel_properties.init_hitscan,
        this->voxel_properties.init_draw
    );
    bornUpdateVox(this->voxel_properties.vox, this->get_position(),
        this->spatial_properties.angles.x, this->spatial_properties.angles.y); 
}

void Slime::die()
{
    #if DC_CLIENT
    //dieAnimation();   // TODO
    if (this->voxel_properties.vox != NULL)
    {
        Vec3 position = this->voxel_properties.vox->get_part(0)->get_center();
        Animations::slime_melt(position.x, position.y, position.z);
    }
    #endif

    #if DC_SERVER     
    ObjectState* state = this->state();
    Vec3 position = this->get_position();
    
    slimeDropItem(position);
    dieExplode(state, position);
    this->broadcastDeath();
    #endif
}

void Slime::tick()
{
    #if DC_SERVER
    this->tick_num++;   // TODO -- make this a component
    if (this->spatial_properties.changed)
        this->broadcastState(); // send state packet if state changed
    else if (this->tick_num % network_state_update_interval == 0)
        this->broadcastState(); // send state packet every N ticks

    Vec3 position = this->get_position();

    // die if near agent
    Agent_state* agent = nearest_agent_in_range(position, this->_state.explosion_proximity_radius);
    if (agent != NULL)
    {
        this->health_properties.dead = true;
        return;
    }

    // acquire target
    agent = random_agent_in_range(position, this->_state.motion_proximity_radius);
    if (agent == NULL) return;
    Vec3 agent_position = vec3_init(agent->s.x, agent->s.y, agent->s.z);

    // face the target
    Vec3 angles = this->get_angles();
    float theta = tickOrientToPointTheta(agent_position, position); // only rotate in x
    this->set_angles(theta, angles.y, angles.z);

    // move towards target
    position = tickMoveToPoint(agent_position, position, this->get_momentum());       // vector between agent and slime
    this->set_position(position.x, position.y, position.z); // move slime position by velocity
    #endif
}

void Slime::update()
{
    updateVox(
        this->voxel_properties.vox, this->get_position(),
        this->spatial_properties.angles, this->spatial_properties.changed
    );
    this->spatial_properties.set_changed(false);
}

// TODO -- generalize dat system
void slimeDropItem(Vec3 position)
{   // TODO -- some dat format for thiss
    #if DC_SERVER
    const float drop_probability = 0.66f;
    float p = randf();
    if (p > drop_probability) return;
    
    const int n_types = 2;    
    Object_types types[n_types] = {
        OBJ_TYPE_LASER_REFILL,
        OBJ_TYPE_GRENADE_REFILL
    };
    Object_types type = types[randrange(0,n_types-1)];
    const float mom = 5.0f;
    ObjectPolicyInterface* obj = ServerState::object_list->create(type,
        position.x, position.y, position.z+1.0f,
        (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom
    );
    if (obj != NULL)
        obj->born();
    #endif
}

void populate_slimes(int n_max)
{   // regenerates slimes up to a maximum
    //#define TYPE Slime
    //Object_types type = OBJ_TYPE_SLIME;
    #define TYPE Box
    Object_types type = OBJ_TYPE_MONSTER_BOX;
    int n_slimes = STATE::object_list->get_object_count(type);
    n_max -= n_slimes;
    //printf("Creating %d slimes\n", n_max);
    if (n_max <= 0) return;
    int x,y,z;
    TYPE* s;
    for (int i=0; i<n_max; i++)
    {
        x = randrange(0,map_dim.x-1);
        y = randrange(0,map_dim.y-1);
        z = _get_highest_open_block(x,y);
        
        #if DC_CLIENT
        z = map_dim.z-1;
        #endif

        s = (TYPE*)STATE::object_list->create(type, x+0.5, y+0.5, z);
        if (s != NULL)
            s->born();
    }
    #undef TYPE
}

}   // Monsters
