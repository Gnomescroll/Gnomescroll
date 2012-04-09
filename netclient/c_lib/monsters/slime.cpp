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
    /*
    #if DC_SERVER
    this->tick_num++;
    if (this->spatial_properties.changed)
        this->broadcastState();
    else if (this->tick_num % network_state_update_interval == 0)
        this->broadcastState();

    // ADD THIS
    //void tickPeriodicBroadcast(ObjectPolicyInterface* obj, int id, int tick_num);
    tickPeriodicBroadcast(this, this->_state.id, this->tick_num);
    //tickProximity();
    //if (this->proximity_properties.near) tickExplode();
    #endif

    //tickProximity

    //this->acquire_target();
    //this->move_to_location(); // increments pos by vel
    //this->face_location();    // orients at direction
    
    // find nearby players
    // if nearby, move toward it

    const float r = 15.0f;
    const float speed = 0.25f;

    if (this->vox == NULL) return;
    // update nearby agents
    STATE::agent_list->objects_within_sphere(this->x, this->y, this->z, r);
    int n_nearby = STATE::agent_list->n_filtered;
    if (n_nearby == 0) return;

    // check if any agent in explode radius
    int i = 0;
    Agent_state* agent = STATE::agent_list->filtered_objects[i++];
    while (agent->status.team == 0) // skip viewer agents
        agent = STATE::agent_list->filtered_objects[i++];
    float dist = STATE::agent_list->filtered_object_distances[i-1];
    if (dist < this->vox->largest_radius()*0.5f)
    {
        agent = STATE::agent_list->filtered_objects[0];
        const int slime_dmg = 20; // TODO
        // blow up, damage player
        agent->status.apply_damage(slime_dmg, this->id, this->type);
        this->health = 0;
    }
    
    // target random nearby player
    i = randrange(0,n_nearby-1);
    agent = STATE::agent_list->filtered_objects[i];
    if (agent == NULL) return;
    
    // determine velocity tick
    float a,b,c;
    float vec[2];
    a = agent->s.x - this->x;
    b = agent->s.y - this->y;
    c = agent->s.z - this->z;
    float len = sqrt(a*a + b*b + c*c);
    a /= len;
    b /= len;
    c /= len;
    vec[0] = a;
    vec[1] = b;
    a *= speed;
    b *= speed;
    c *= speed;

    // apply velocity
    this->x += a;
    this->y += b;
    this->z += c;

    // calculate rotation deltas
    // THETA = acos( (A.B) / (|A|*|B|) )
    float dtheta;
    float dot, alen, blen;

    float ftmp[2], vtmp[2];
    ftmp[0] = 1.0;
    ftmp[1] = 0.0;
    vtmp[0] = vec[0];
    vtmp[1] = vec[1];

    // calculate theta
    dot = ftmp[0]*vtmp[0] + ftmp[1]*vtmp[1];
    alen = ftmp[0]*ftmp[0] + ftmp[1]*ftmp[1];
    blen = vtmp[0]*vtmp[0] + vtmp[1]*vtmp[1];
    dtheta = acos ( dot / sqrt(alen*blen) );

    // orient towards player
    this->theta = dtheta;
    */
}

void Slime::update()
{
    updateVox(
        this->voxel_properties.vox, this->get_position(),
        this->spatial_properties.angles.x, this->spatial_properties.angles.y
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
