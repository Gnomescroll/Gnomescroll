#pragma once

#include <c_lib/physics/verlet.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/entity/entity.hpp>
#include <c_lib/items/packets.hpp>
#include <c_lib/components/billboard_sprite.hpp>

//forward decl
#if DC_CLIENT
namespace ClientState
{
    extern BillboardSpriteList* billboard_sprite_list;
    extern ColoredMinivoxList* colored_minivox_list;
}
#endif

namespace ItemDrops
{

/* State */

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_TEXTURE_SCALE = 1.0f;
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
    int agent_id = object->nearest_agent_in_range(object->get_position(), pickup_radius);
    if (agent_id >= 0 && STATE::agent_list->agent_pickup_item(agent_id, state->type))
    {   // was picked up, die
        object->was_picked_up(agent_id);
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
        int nearest_agent_in_range(const Vec3 p, const float radius);
        
    PickupComponent()
    : pickup_radius(1.0f), picked_up_by(-1)
    {}

    ~PickupComponent()
    {
    }
};

/* Initializers */

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

void initialize_sprite_properties(Object_types type, BillboardSpriteProperties* obj)
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            obj->texture_index = GRENADE_REFILL_TEXTURE_ID;
            obj->texture_scale = GRENADE_REFILL_TEXTURE_SCALE;
            break;

        case OBJ_TYPE_LASER_REFILL:
            obj->texture_index = LASER_REFILL_TEXTURE_ID;
            obj->texture_scale = LASER_REFILL_TEXTURE_SCALE;
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

typedef ObjectInterface
< VerletState, object_create_momentum_StoC, object_state_momentum_StoC >
PickupInterface;

class PickupObject: public PickupComponent, public PickupInterface
{
    public:
    PickupObject(Object_types type, int id)
    : PickupComponent(), PickupInterface()
    {   // TODO: constants should be loaded via dat
        this->_state.type = type;
        this->_state.id = id;
        this->_state.mass = DEFAULT_PICKUP_ITEM_MASS;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;

        this->pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;

        initialize_pickup_object(type, this->state());
    }

    ~PickupObject()
    {
    }

    void tick()
    {
        tickVerletBounce(&this->verlet_properties.vp, this->state()->damp);
        tickPickup(this->state(), this, this->pickup_radius);
        tickTTL(this->state(), this);
    }

    void update() {}

    void born()
    {
        this->broadcastCreate();
    }

    void die()
    {
        ObjectState* state = this->state();
        diePickup(state->id, state->type, this->picked_up_by, this->broadcast_death);
    }
};

class PickupObjectSprite: public PickupObject, public BillboardSpriteComponent
{
    public:

    PickupObjectSprite(Object_types type, int id)
    : PickupObject(type, id)
    {
        #if DC_CLIENT
        initialize_sprite_properties(type, &this->sprite_properties);
        this->sprite_properties.obj = this;
        ClientState::billboard_sprite_list->register_object(&this->sprite_properties);
        #endif
    }

    ~PickupObjectSprite()
    {
        #if DC_CLIENT
        ClientState::billboard_sprite_list->unregister_object(&this->sprite_properties);
        #endif
    }

    void draw()
    {
        drawBillboardSprite(this->get_position(), this->sprite_properties.texture_index, this->sprite_properties.texture_scale);
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
        tickRotate(this, this->minivox_properties.dtheta_speed, this->minivox_properties.dphi_speed);
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
