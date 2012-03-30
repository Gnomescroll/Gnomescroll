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
    protected:
        inline void tick(ObjectState* state, Object* object)
        {Super::tick(state, object);}
};

template <class Object>
class TickAnchor
{
    protected:
        inline void tick(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class DrawCombiner: public Super
{
    protected:
        inline void draw(ObjectState* state, Object* object)
        {Super::draw(state, object);}
};

template <class Object>
class DrawAnchor
{
    protected:
        inline void draw(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class UpdateCombiner: public Super
{
    protected:
        inline void update(ObjectState* state, Object* object)
        {Super::update(state, object);}
};

template <class Object>
class UpdateAnchor
{
    protected:
        inline void update(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class BornCombiner: public Super
{
    protected:
        inline void born(ObjectState* state, Object* object)
        {Super::born(state, object);}
};

template <class Object>
class BornAnchor
{
    protected:
        inline void born(ObjectState* state, Object* object) {/*Empty*/}
};

template <class Super, class Object>
class DieCombiner: public Super
{
    protected:
        inline void die(ObjectState* state, Object* object)
        {Super::die(state, object);}
};

template <class Object>
class DieAnchor
{
    protected:
        inline void die(ObjectState* state, Object* object) {/*Empty*/}
};

/***************************/

// Macros for common behaviour configurations
// All behaviours must terminate with these

#define NoTick(OBJECT) TickAnchor<OBJECT>
#define NoDraw(OBJECT) DrawAnchor<OBJECT>
#define NoUpdate(OBJECT) UpdateAnchor<OBJECT>
#define NoBorn(OBJECT) BornAnchor<OBJECT>
#define NoDie(OBJECT) DieAnchor<OBJECT>
