#pragma once

#include <c_lib/entity/state.hpp>

#if DC_CLIENT
#include <c_lib/camera/camera.hpp>
#endif

//forward declaration
namespace ServerState
{
    extern Agent_list* agent_list;
};

template <class Super, class Object>
class TickCombiner: public Super
{
    public:
        inline void tick(ObjectState* state, Object* object)
        {Super::tick(state, object);}
};

template <class Object>
class TickAnchor
{
    public:
        inline void tick(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class DrawCombiner: public Super
{
    public:
        inline void draw(ObjectState* state, Object* object)
        {Super::draw(state, object);}
};

template <class Object>
class DrawAnchor
{
    public:
        inline void draw(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class UpdateCombiner: public Super
{
    public:
        inline void update(ObjectState* state, Object* object)
        {Super::update(state, object);}
};

template <class Object>
class UpdateAnchor
{
    public:
        inline void update(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class BornCombiner: public Super
{
    public:
        inline void born(ObjectState* state, Object* object)
        {Super::born(state, object);}
};

template <class Object>
class BornAnchor
{
    public:
        inline void born(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class DieCombiner: public Super
{
    public:
        inline void die(ObjectState* state, Object* object)
        {Super::die(state, object);}
};

template <class Object>
class DieAnchor
{
    public:
        inline void die(ObjectState* state, Object* object) {/*Empty*/}
};

// TODO : move
template <class Super, class Object>
class DieBlowup: public Super
{
    public:
    inline void die(ObjectState* state, Object* object)
    {
        Super::die(state, object);
    }

};

// TODO : move
template <class Super, class Object>
class TickParticle: public Super
{
    public:
    inline void tick(ObjectState* state, Object* object)
    {
        Verlet::bounce(state->vp, state->damp);
        Super::tick(state, object);
    }
};

// TODO : move
template <class Super, class Object>
class TickTTL: public Super
{
    public:
    inline void tick(ObjectState* state, Object* object)
    {
        if (state->ttl >= 0)
            state->ttl++;
        Super::tick(state, object);
    }
};

// TODO: move
template <class Super, class Object>
class DrawBillboardSprite: public Super
{
    public:
    inline void draw(ObjectState* state, Object* object)
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
        Super::draw(state, object);
    }
};

/***************************/

// Macros for common behaviour configurations
// All behaviours must terminate with these

#define NoTick(OBJECT) TickAnchor<OBJECT>
#define NoDraw(OBJECT) DrawAnchor<OBJECT>
#define NoUpdate(OBJECT) UpdateAnchor<OBJECT>
#define NoBorn(OBJECT) BornAnchor<OBJECT>
#define NoDie(OBJECT) DieAnchor<OBJECT>
