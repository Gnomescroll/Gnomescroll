#pragma once

#include <c_lib/entity/state.hpp>
#include <c_lib/items/packets.hpp>

#if DC_CLIENT
#include <c_lib/camera/camera.hpp>
#endif

//forward declaration
namespace ServerState
{
    extern Agent_list* agent_list;
};

template <class Super, typename State>
class TickCombiner: public Super
{
    public:
        inline void tick(State* state)
        {Super::tick(state);}
};

template <typename State>
class TickAnchor
{
    public:
        inline void tick(State* state) {/*Empty*/}
};

template <class Super, typename State>
class DrawCombiner: public Super
{
    public:
        inline void draw(State* state)
        {Super::draw(state);}
};

template <typename State>
class DrawAnchor
{
    public:
        inline void draw(State* state) {/*Empty*/}
};

template <class Super, typename State>
class BornCombiner: public Super
{
    public:
        inline void born(State* state)
        {Super::born(state);}
};


template <typename State>
class BornAnchor
{
    public:
        inline void born(State* state) {/*Empty*/}
};

template <class Super, typename State>
class DieCombiner: public Super
{
    public:
        inline void die(State* state)
        {Super::die(state);}
};

template <typename State>
class DieAnchor
{
    public:
        inline void die(State* state) {/*Empty*/}
};

template <class Super, typename State>
class DieBlowup: public Super
{
    public:
    inline void die(State* state)
    {
        Super::die(state);
    }

};

template <class Super, typename State>
class DiePickup: public Super
{
    public:
    inline void die(State* state)
    {
        #if DC_SERVER
        if (state->broadcast_death)
        {
            item_picked_up_StoC msg;
            msg.type = state->type;
            msg.id = state->id;
            msg.agent_id = state->picked_up_by;
            msg.broadcast();
        }
        #endif
        Super::die(state);
    }

};

template <class Super, typename State>
class BornPickup: public Super
{
    public:
    inline void born(State* state)
    {
        #if DC_SERVER
        item_create_StoC msg;
        msg.type = state->type;
        msg.id = state->id;
        msg.x = state->vp->p.x;
        msg.y = state->vp->p.y;
        msg.z = state->vp->p.z;
        Vec3 m = state->vp->get_momentum();
        msg.mx = m.x;
        msg.my = m.y;
        msg.mz = m.z;
        msg.broadcast();
        #endif
        Super::born(state);
    }

};

template <class Super, typename State>
class TickParticle: public Super
{
    public:
    inline void tick(State* state)
    {
        Verlet::bounce(state->vp, state->damp);
        Super::tick(state);
    }
};

template <class Super, typename State>
class TickTTL: public Super
{
    public:
    inline void tick(State* state)
    {
        state->ttl++;
        Super::tick(state);
    }
};

template <class Super, typename State>
class TickPickup: public Super
{
    public:
    inline void tick(State* state)
    {
        #if DC_SERVER
        int agent_id = state->object->nearest_agent_in_range(state->vp->p, state->pickup_radius);
        if (agent_id >= 0 && STATE::agent_list->agent_pickup_item(agent_id, state->type))
        {   // was picked up, die
            state->object->was_picked_up(agent_id);
            state->ttl = state->ttl_max;
        }
        #endif
        Super::tick(state);
    }

};

template <class Super, typename State>
class DrawBillboardSprite: public Super
{
    public:
    inline void draw(State* state)
    {
        #if DC_CLIENT
        Vec3 v = state->vp->p;
        v.z += state->texture_scale;
        
        if (current_camera == NULL
        || !current_camera->in_view(v.x, v.y, v.z))
            return;

        Vec3 up = vec3_init(
            model_view_matrix[0]*state->texture_scale,
            model_view_matrix[4]*state->texture_scale,
            model_view_matrix[8]*state->texture_scale
        );
        Vec3 right = vec3_init(
            model_view_matrix[1]*state->texture_scale,
            model_view_matrix[5]*state->texture_scale,
            model_view_matrix[9]*state->texture_scale
        );

        float tx_min, tx_max, ty_min, ty_max;
        tx_min = (float)(state->texture_index%16)* (1.0/16.0);
        tx_max = tx_min + (1.0/16.0);
        ty_min = (float)(state->texture_index/16)* (1.0/16.0);
        ty_max = ty_min + (1.0/16.0);

        Vec3 p = vec3_sub(v, vec3_add(right, up));
        glTexCoord2f(tx_min,ty_max);
        glVertex3f(p.x, p.y, p.z);

        p = vec3_add(v, vec3_sub(up, right));
        glTexCoord2f(tx_max,ty_max);
        glVertex3f(p.x, p.y, p.z);

        p = vec3_add(v, vec3_add(up, right));
        glTexCoord2f(tx_max,ty_min);
        glVertex3f(p.x, p.y, p.z);

        p = vec3_add(v, vec3_sub(right, up));
        glTexCoord2f(tx_min,ty_min);
        glVertex3f(p.x, p.y, p.z);
        #endif
        Super::draw(state);
    }

};

/***************************/

#define NoTick(STATE) TickAnchor<STATE>
#define NoDraw(STATE) DrawAnchor<STATE>
#define NoBorn(STATE) BornAnchor<STATE>
#define NoDie(STATE) DieAnchor<STATE>
