#pragma once

#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/objects/common/interface/entity.hpp>
#include <c_lib/objects/common/net/packets.hpp>
#include <c_lib/items/constants.hpp>

//forward decl
#if DC_CLIENT
#include <c_lib/objects/components/sprite/sprite.hpp>
#include <c_lib/objects/components/minivox/colored_minivox.hpp>
#include <c_lib/objects/components/minivox/textured_minivox.hpp>
#include <c_lib/draw/lists.hpp>
#include <c_lib/t_map/glsl/texture.hpp>
#endif

namespace ItemDrops
{

/* Behaviour */

inline void diePickup(int id, Object_types type, int picked_up_by, bool broadcast_death)
{
    #if DC_SERVER
    if (picked_up_by < 0)
        return;

    if (broadcast_death)
    {
        object_picked_up_StoC msg;
        msg.id = id;
        msg.type = type;
        msg.agent_id = picked_up_by;
        msg.broadcast();
    }
    #endif
}

template <class Object>
void tickPickup(ObjectState* state, Object* object, float pickup_radius)
{
    #if DC_SERVER
    Agent_state* agent = nearest_agent_in_range(object->get_position(), pickup_radius);
    if (agent != NULL && agent->status.gain_item(state->id, state->type, state->subtype))
    {   // was picked up, die
        object->was_picked_up(agent->id);
        state->ttl = state->ttl_max;
    }
    #endif
}

/* Reusable Component */

class PickupComponent
{
    public:
        float pickup_radius;
        int picked_up_by;
        bool broadcast_death;

        void was_picked_up(const int agent_id);
        
    PickupComponent()
    : pickup_radius(1.0f), picked_up_by(-1)
    {}

    ~PickupComponent()
    {
    }
};

/* Initializers */

// TODO -- deprecate for physics initializer
void initialize_pickup_object(Object_types type, int subtype, ObjectState* state)
{
    switch (subtype)
    {
        case GRENADE_REFILL:
            state->mass = GRENADE_REFILL_MASS;
            state->ttl_max = GRENADE_REFILL_TTL;
            state->damp = GRENADE_REFILL_DAMP;
            break;

        case LASER_REFILL:
            state->mass = LASER_REFILL_MASS;
            state->ttl_max = LASER_REFILL_TTL;
            state->damp = LASER_REFILL_DAMP;
            break;

        default:
            state->mass = DEFAULT_PICKUP_ITEM_MASS;
            state->ttl_max = DEFAULT_PICKUP_ITEM_TTL;
            state->damp = DEFAULT_PICKUP_ITEM_DAMP;            
            break;
    }
}

void initialize_pickup_properties(Object_types type, int subtype, PickupComponent* obj)
{
    switch (type)
    {
        case OBJ_TYPE_GEMSTONE:
            obj->pickup_radius = GEMSTONE_PICKUP_RADIUS;
            break;
        default: return;
    }
}

#if DC_CLIENT
void initialize_sprite_properties(Object_types type, int subtype, SpriteProperties* obj)
{
    int sprite_index = get_object_type_sprite_index(type, subtype);
    obj->sprite_index = sprite_index;
    switch (type)
    {
        case OBJ_TYPE_REFILL:
            switch (subtype)
            {
                case GRENADE_REFILL:
                    obj->scale = GRENADE_REFILL_TEXTURE_SCALE;
                    break;
                case LASER_REFILL:
                    obj->scale = LASER_REFILL_TEXTURE_SCALE;
                    break;                
                default:
                    obj->scale = DEFAULT_PICKUP_ITEM_SCALE;
                    break;
            }
            break;
            
        case OBJ_TYPE_GEMSTONE:
            obj->scale = GEMSTONE_SCALE;
            break;

        case OBJ_TYPE_MEAT:
            obj->scale = MEAT_SCALE;
            break;

        default:
            obj->scale =DEFAULT_PICKUP_ITEM_SCALE;
            break;
    }
}

void initialize_colored_minivox_properties(Object_types type, int subtype, ColoredMinivoxProperties* obj)
{
    switch (type)
    {
        case OBJ_TYPE_BLOCK_DROP:
            obj->size = BLOCK_DROP_SIZE;
            switch (subtype)
            {
                case DIRT:
                    obj->color = DIRT_COLOR;
                    break;
                case STONE:
                    obj->color = STONE_COLOR;
                    break;

                default:
                    obj->color = BLOCK_DROP_COLOR;
                    break;
            }
            break;
            
        default: break;
    }
    obj->dtheta_speed = MINIVOX_ITEM_ROTATION_THETA;
    obj->dphi_speed = MINIVOX_ITEM_ROTATION_PHI;
}

int get_cached_cube_texture_indices(int cube_type)
{
    static int DIRT_TEXTURE_INDEX = -1;
    static int STONE_TEXTURE_INDEX = -1;
    static int SOFT_ROCK_TEXTURE_INDEX = -1;
    static int MEDIUM_ROCK_TEXTURE_INDEX = -1;
    static int HARD_ROCK_TEXTURE_INDEX = -1;
    static int INFECTED_ROCK_TEXTURE_INDEX = -1;

    static int ERROR_TEXTURE_INDEX = -1;
    if (ERROR_TEXTURE_INDEX < 0)
        ERROR_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"error_block");

    int texture_index = ERROR_TEXTURE_INDEX;
    switch (cube_type)
    {
        case DIRT:
            if (DIRT_TEXTURE_INDEX < 0)
                DIRT_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"dirt");
            texture_index = DIRT_TEXTURE_INDEX;
            break;
        case STONE:
            if (STONE_TEXTURE_INDEX < 0)
                STONE_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"stone");
            texture_index = STONE_TEXTURE_INDEX;
            break;
        case SOFT_ROCK:
            if (SOFT_ROCK_TEXTURE_INDEX < 0)
                SOFT_ROCK_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"soft_rock");
            texture_index = SOFT_ROCK_TEXTURE_INDEX;
            break;
        case MEDIUM_ROCK:
            if (MEDIUM_ROCK_TEXTURE_INDEX < 0)
                MEDIUM_ROCK_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"medium_rock");
            texture_index = MEDIUM_ROCK_TEXTURE_INDEX;
            break;
        case HARD_ROCK:
            if (HARD_ROCK_TEXTURE_INDEX < 0)
                HARD_ROCK_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"hard_rock");
            texture_index = HARD_ROCK_TEXTURE_INDEX;
            break;
        case INFECTED_ROCK:
            if (INFECTED_ROCK_TEXTURE_INDEX < 0)
                INFECTED_ROCK_TEXTURE_INDEX = t_map::get_cube_primary_texture_index((char*)"infested_rock");
            texture_index = INFECTED_ROCK_TEXTURE_INDEX;
            break;
        default:
            break;
    }
    return texture_index;
}

void initialize_textured_minivox_properties(Object_types type, int subtype, TexturedMinivoxProperties* obj)
{
    switch (type)
    {
        case OBJ_TYPE_BLOCK_DROP:
            obj->size = BLOCK_DROP_SIZE;
            obj->texture_index = get_cached_cube_texture_indices(subtype);
            break;
            
        default: break;
    }
    obj->dtheta_speed = MINIVOX_ITEM_ROTATION_THETA;
    obj->dphi_speed = MINIVOX_ITEM_ROTATION_PHI;
    obj->pixel_width = BLOCK_DROP_TEXTURED_PIXEL_WIDTH;
}
#endif

/* Composition */

class PickupObject: public PickupComponent, public ObjectStateLayer
{
    public:
        VerletComponent spatial;
    
    PickupObject(int id)
    : PickupComponent(), ObjectStateLayer(Objects::create_packet_momentum, Objects::state_packet_momentum, Objects::owned_none, Objects::team_none, Objects::health_none, &spatial)
    {   // TODO: constants should be loaded via dat
        this->_state.id = id;
        this->_state.mass = DEFAULT_PICKUP_ITEM_MASS;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;

        this->pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;

    }

    ~PickupObject()
    {
    }

    void tick()
    {
        ObjectState* state = this->state();
        this->spatial.verlet_bounce(state->damp);
        tickPickup(state, this, this->pickup_radius);
        tickTTL(state);
    }

    void update() {}

    void born(int subtype)
    {
        this->_state.subtype = subtype;
        ObjectState* state = this->state();
        initialize_pickup_object(state->type, subtype, state);
        initialize_pickup_properties(state->type, subtype, this);
        this->broadcastCreate();
    }

    void die()
    {
        ObjectState* state = this->state();
        diePickup(state->id, state->type, this->picked_up_by, this->broadcast_death);
    }
};

class PickupObjectSprite:
    #if DC_CLIENT
    public SpriteComponent,
    #endif
    public PickupObject
{
    public:

    PickupObjectSprite(int id)
    : PickupObject(id)
    {
        #if DC_CLIENT
        this->sprite_properties.obj = this;
        #endif
    }

    void born(int subtype)
    {
        PickupObject::born(subtype);
        #if DC_CLIENT
        initialize_sprite_properties(this->state()->type, subtype, &this->sprite_properties);
        Draw::sprite_list->register_object(&this->sprite_properties);
        #endif
    }

    ~PickupObjectSprite()
    {
        #if DC_CLIENT
        Draw::sprite_list->unregister_object(&this->sprite_properties);
        #endif
    }
};

class PickupObjectColoredMinivox:
    #if DC_CLIENT
    public ColoredMinivoxComponent,
    #endif
    public PickupObject
{
    public:

    PickupObjectColoredMinivox(int id)
    : PickupObject(id)
    {
        #if DC_CLIENT
        this->set_rotation(0,0);
        this->minivox_properties.obj = this;
        #endif
    }

    ~PickupObjectColoredMinivox()
    {
        #if DC_CLIENT
        Draw::colored_minivox_list->unregister_object(&this->minivox_properties);
        #endif
    }

    void born(int subtype)
    {
        PickupObject::born(subtype);
        #if DC_CLIENT
        ObjectState* state = this->state();
        initialize_colored_minivox_properties(state->type, state->subtype, &this->minivox_properties);
        Draw::colored_minivox_list->register_object(&this->minivox_properties);
        #endif
    }

    void tick()
    {
        #if DC_CLIENT
        this->delta_rotation();
        #endif
        PickupObject::tick();
    }
};

class PickupObjectTexturedMinivox:
    #if DC_CLIENT
    public TexturedMinivoxComponent,
    #endif
    public PickupObject
{
    public:

    PickupObjectTexturedMinivox(int id)
    : PickupObject(id)
    {
        #if DC_CLIENT
        this->set_rotation(0,0);
        this->minivox_properties.obj = this;
        #endif
    }

    ~PickupObjectTexturedMinivox()
    {
        #if DC_CLIENT
        Draw::textured_minivox_list->unregister_object(&this->minivox_properties);
        #endif
    }

    void born(int subtype)
    {
        PickupObject::born(subtype);
        #if DC_CLIENT
        ObjectState* state = this->state();
        initialize_textured_minivox_properties(state->type, state->subtype, &this->minivox_properties);
        this->set_texture();
        Draw::textured_minivox_list->register_object(&this->minivox_properties);
        #endif
    }

    void tick()
    {
        #if DC_CLIENT
        this->delta_rotation();
        #endif
        PickupObject::tick();
    }
};

ObjectPolicyInterface* create_block_drop(int subtype, int id)
{
    ObjectPolicyInterface* obj = NULL;
    switch (subtype)
    {
        case DIRT:
        case STONE:
            obj = new PickupObjectColoredMinivox(id);
            break;

        case SOFT_ROCK:
        case MEDIUM_ROCK:
        case HARD_ROCK:
        case INFECTED_ROCK:
            obj = new PickupObjectTexturedMinivox(id);
            break;

        default: break;
    }
    return obj;

}

} // ItemDrops
