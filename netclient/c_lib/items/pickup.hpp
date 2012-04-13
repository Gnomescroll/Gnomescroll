#pragma once

#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/objects/common/interface/entity.hpp>
#include <c_lib/objects/common/net/packets.hpp>
#include <c_lib/objects/components/sprite/sprite.hpp>

#include <c_lib/items/constants.hpp>


using namespace ItemDrops;

//forward decl
#if DC_CLIENT
namespace ClientState
{
    extern SpriteList* sprite_list;
    extern ColoredMinivoxList* colored_minivox_list;
}
#endif

namespace ItemDrops
{

/* State */

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

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
    if (agent != NULL && agent->status.gain_item(state->id, state->type))
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

// TODO -- deprecate for phyics initializer
void initialize_pickup_object(Object_types type, ObjectState* state)
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            state->mass = GRENADE_REFILL_MASS;
            state->ttl_max = GRENADE_REFILL_TTL;
            state->damp = GRENADE_REFILL_DAMP;
            break;

        case OBJ_TYPE_LASER_REFILL:
            state->mass = LASER_REFILL_MASS;
            state->ttl_max = LASER_REFILL_TTL;
            state->damp = LASER_REFILL_DAMP;
            break;

        default: return;
    }
}

void initialize_pickup_properties(Object_types type, PickupComponent* obj)
{
    switch (type)
    {
        case OBJ_TYPE_MEAT:
        case OBJ_TYPE_MALACHITE:
        case OBJ_TYPE_RUBY:
        case OBJ_TYPE_TURQUOISE:
        case OBJ_TYPE_SILVER:
        case OBJ_TYPE_AMETHYST:
        case OBJ_TYPE_JADE:
        case OBJ_TYPE_ONYX:
            obj->pickup_radius = GEMSTONE_PICKUP_RADIUS;
            break;
            
        default: return;
    }
}


void initialize_sprite_properties(Object_types type, SpriteProperties* obj)
{
    int sprite_index = get_object_type_sprite_index(type);
    obj->sprite_index = sprite_index;
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            obj->scale = GRENADE_REFILL_TEXTURE_SCALE;
            break;
        case OBJ_TYPE_LASER_REFILL:
            obj->scale = LASER_REFILL_TEXTURE_SCALE;
            break;

        case OBJ_TYPE_MALACHITE:
            obj->scale = GEMSTONE_SCALE;
            break;
        case OBJ_TYPE_RUBY:
            obj->scale = GEMSTONE_SCALE;
            break;
        case OBJ_TYPE_TURQUOISE:
            obj->scale = GEMSTONE_SCALE;
            break;
        case OBJ_TYPE_SILVER:
            obj->scale = GEMSTONE_SCALE;
            break;
        case OBJ_TYPE_AMETHYST:
            obj->scale = GEMSTONE_SCALE;
            break;
        case OBJ_TYPE_JADE:
            obj->scale = GEMSTONE_SCALE;
            break;
        case OBJ_TYPE_ONYX:
            obj->scale = GEMSTONE_SCALE;
            break;

        case OBJ_TYPE_MEAT:
            obj->scale = MEAT_SCALE;
            break;

        default: return;
    }
}

void initialize_minivox_properties(Object_types type, MinivoxProperties* obj)
{
    switch (type)
    {
        case OBJ_TYPE_DIRT:
            obj->color = DIRT_COLOR;
            obj->size = DIRT_SIZE;
            break;
        case OBJ_TYPE_STONE:
            obj->color = STONE_COLOR;
            obj->size = STONE_SIZE;
            break;
            
        default: return;
    }
    obj->dtheta_speed = MINIVOX_ITEM_ROTATION_THETA;
    obj->dphi_speed = MINIVOX_ITEM_ROTATION_PHI;
}

/* Composition */

class PickupObject: public PickupComponent, public ObjectStateLayer
{
    public:
        VerletComponent spatial;
    
    PickupObject(Object_types type, int id)
    : PickupComponent(), ObjectStateLayer(Objects::create_packet_momentum, Objects::state_packet_momentum, Objects::owned_none, Objects::team_none, Objects::health_none, &spatial)
    {   // TODO: constants should be loaded via dat
        this->_state.type = type;
        this->_state.id = id;
        this->_state.mass = DEFAULT_PICKUP_ITEM_MASS;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;

        this->pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;

        initialize_pickup_object(type, this->state());
        initialize_pickup_properties(type, this);
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
        this->broadcastCreate();
    }

    void die()
    {
        ObjectState* state = this->state();
        diePickup(state->id, state->type, this->picked_up_by, this->broadcast_death);
    }
};

class PickupObjectSprite: public PickupObject, public SpriteComponent
{
    public:

    PickupObjectSprite(Object_types type, int id)
    : PickupObject(type, id)
    {
        #if DC_CLIENT
        initialize_sprite_properties(type, &this->sprite_properties);
        this->sprite_properties.obj = this;
        ClientState::sprite_list->register_object(&this->sprite_properties);
        #endif
    }

    ~PickupObjectSprite()
    {
        #if DC_CLIENT
        ClientState::sprite_list->unregister_object(&this->sprite_properties);
        #endif
    }

    void draw()
    {
        drawBillboardSprite(this->get_position(), this->sprite_properties.sprite_index, this->sprite_properties.scale);
    }
};

class PickupObjectMinivox: public PickupObject, public MinivoxComponent
{
    public:

    PickupObjectMinivox(Object_types type, int id)
    : PickupObject(type, id)
    {
        #if DC_CLIENT
        initialize_minivox_properties(type, &this->minivox_properties);
        this->set_rotation(0,0);
        this->minivox_properties.obj = this;
        ClientState::colored_minivox_list->register_object(&this->minivox_properties);
        #endif
    }

    ~PickupObjectMinivox()
    {
        #if DC_CLIENT
        ClientState::colored_minivox_list->unregister_object(&this->minivox_properties);
        #endif
    }

    void tick()
    {
        #if DC_CLIENT
        this->delta_rotation();
        #endif
        PickupObject::tick();
    }

    void draw()
    {
        drawMinivox(
            this->get_position(),
            this->minivox_properties.forward,
            this->minivox_properties.right,
            this->minivox_properties.normal,
            this->minivox_properties.size * 0.5,
            this->minivox_properties.color
        );
    }
};

} // ItemDrops
