#pragma once

#include <c_lib/physics/verlet.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/entity/entity.hpp>
#include <c_lib/items/packets.hpp>

namespace ItemDrops
{

/* State */

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_TEXTURE_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

/* Behaviour */

template <class Object>
inline void diePickup(ObjectState* state, Object* object)
{
    #if DC_SERVER
    if (state->broadcast_death)
    {
        object_picked_up_StoC msg;
        msg.type = state->type;
        msg.id = state->id;
        msg.agent_id = state->picked_up_by;
        msg.broadcast();
    }
    #endif
}

template <class Object>
void tickPickup(ObjectState* state, Object* object)
{
    #if DC_SERVER
    int agent_id = object->nearest_agent_in_range(state->vp->p, state->pickup_radius);
    if (agent_id >= 0 && STATE::agent_list->agent_pickup_item(agent_id, state->type))
    {   // was picked up, die
        object->was_picked_up(state, agent_id);
        state->ttl = state->ttl_max;
    }
    #endif
}

/* Reusable Component */

class PickupComponent
{
    public:
    void was_picked_up(ObjectState* state, const int agent_id);
    int nearest_agent_in_range(const Vec3 p, const float radius);
    PickupComponent(){}
};

/* Composition */

typedef ObjectInterface
< object_create_vel_StoC, object_state_vel_StoC >
PickupInterface;

class PickupObject: public PickupComponent, public PickupInterface
{
    public:
    PickupObject(int id)
    : PickupComponent(), PickupInterface()
    {   // TODO: constants should be loaded via dat
        this->_state.id = id;
        this->_state.pickup = true;
        this->_state.mass = DEFAULT_PICKUP_ITEM_MASS;
        this->_state.create_particle(0,0,0,0,0,0);
        this->_state.pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;
        this->_state.texture_scale = DEFAULT_PICKUP_ITEM_TEXTURE_SCALE;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;
    }

    /* Interface */

    void tick()
    {
        tickVerletBounce(this->state(), this);
        tickPickup(this->state(), this);
        tickTTL(this->state(), this);
    }

    void update() {}

    void draw()
    {
        drawBillboardSprite(this->state(), this);
    }

    void born()
    {
        bornCreateMessage(this->state(), this);
    }

    void die()
    {
        diePickup(this->state(), this);
    }
};

} // ItemDrops
