#include "monsters.hpp"

#include <math.h>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/common/random.h>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

VoxDat slime_vox_dat;

void Slime::load_data(int id)
{
    this->_state.id = id;
    this->_state.type = OBJ_TYPE_SLIME;

    this->_state.coin_rule = COINS_ANYONE;

    this->_state.explosion_radius = SLIME_EXPLOSION_RADIUS;
    this->_state.explosion_damage = SLIME_EXPLOSION_DAMAGE;
    this->_state.explosion_proximity_radius = SLIME_EXPLOSION_PROXIMITY_RADIUS;

    this->voxel_properties.init_hitscan = true;
    this->voxel_properties.init_draw = true;
    this->voxel_properties.vox_dat = &slime_vox_dat;

    this->spatial_properties.camera_height = SLIME_CAMERA_HEIGHT;
    this->spatial_properties.height = SLIME_HEIGHT;

    this->health_properties.health = SLIME_HEALTH;
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
    
    slimeDropItem(this->get_position());
    dieExplode(state, position);
    this->broadcastDeath();
    #endif
}

void Slime::tick()
{
    #if DC_SERVER
    this->tick_num++;
    if (this->spatial_properties.changed) // TODO -- changed
        this->broadcastState(); // send state packet if state changed
    else if (this->tick_num % network_state_update_interval == 0)
        this->broadcastState(); // send state packet every N ticks

    // die if near agent
    int agent_id = nearest_agent_in_range(this->get_position(), this->_state.explosion_proximity_radius);
    if (agent_id >= 0) this->health_properties.dead = true;

    //this->acquire_target();
    //this->move_to_location(); // increments pos by vel
    //this->face_location();    // orients at direction
    
    // find nearby players
    // if nearby, move toward it

    Vec3 position = this->get_position();

    const float r = 15.0f;
    // update nearby agents
    STATE::agent_list->objects_within_sphere(position.x, position.y, position.z, r);
    int n_nearby = STATE::agent_list->n_filtered;
    if (n_nearby == 0) return;

    // target random nearby player
    int i = randrange(0, n_nearby-1);
    Agent_state* agent = STATE::agent_list->filtered_objects[i];
    if (agent == NULL) return;
    
    const float speed = 0.25f;
    Vec3 agent_position = vec3_init(agent->s.x, agent->s.y, agent->s.z);
    Vec3 velocity = vec3_sub(agent_position, position);       // vector between agent and slime
    normalize_vector(&velocity);
    Vec3 normalized_velocity = vec3_copy(velocity);
    velocity = vec3_scalar_mult(velocity, speed);             //  apply magnitude to velocity
    position = vec3_add(position, velocity);
    this->set_position(position.x, position.y, position.z); // move slime position by velocity

    // calculate rotation deltas
    //struct Vec3 angles = this->get_angles();   // TODO -- angles on object should be vector
    //angles = vec3_init_from_angles(angles.x, angles.y, 0);

    // calculate theta
    // THETA = acos( (A.B) / (|A|*|B|) )
    //float dot = angles.x * normalized_velocity.x + angles.y * normalized_velocity.y;
    //float dtheta = acos(dot);

    // orient towards player
    //Vec3 new_angles = this->get_angles();
    //new_angles.x += dtheta;
    normalized_velocity.z = 0;
    normalize_vector(&normalized_velocity);
    this->set_angles(normalized_velocity.x, normalized_velocity.y, normalized_velocity.z);
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

void populate_slimes(int n_max)
{   // regenerates slimes up to a maximum
    int n_slimes = STATE::object_list->get_object_count(OBJ_TYPE_SLIME);
    n_max -= n_slimes;
    //printf("Creating %d slimes\n", n_max);
    if (n_max <= 0) return;
    int x,y,z;
    Slime* s;
    for (int i=0; i<n_max; i++)
    {
        x = randrange(0,map_dim.x-1);
        y = randrange(0,map_dim.y-1);
        z = _get_highest_open_block(x,y);
        
        #if DC_CLIENT
        z = map_dim.z-1;
        #endif

        s = (Slime*)STATE::object_list->create(OBJ_TYPE_SLIME, x+0.5, y+0.5, z, 0,0,0);
        if (s != NULL)
            s->born();
        else printf("SLIME ISNULL\n");
    }
}

}   // Monsters
