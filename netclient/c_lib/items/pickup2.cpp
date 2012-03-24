#include "pickup2.hpp"

#include <c_lib/state/server_state.hpp>
#include <c_lib/particles/textures.hpp>
#include <c_lib/particles/particles.hpp>

#include <c_lib/physics/verlet.hpp>
#include <c_lib/items/packets.hpp>

namespace ItemDrops
{

template <class Super, typename State>
void TickPickup<Super, State>::tick(State* state)
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

template <class Super, typename State>
void DrawBillboardSprite<Super, State>::draw(State* state)
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

int PickupObject::nearest_agent_in_range(const Vec3 p, const float radius)
{
    int n = STATE::agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
    if (n > 0)
        return STATE::agent_list->filtered_objects[0]->id;
    return -1;
}

// TODO: make born part of the object api 
void PickupObject::born()
{
    #if DC_SERVER
    item_create_StoC msg;
    msg.type = this->state()->type;
    msg.id = this->state()->id;
    msg.x = this->state()->vp->p.x;
    msg.y = this->state()->vp->p.y;
    msg.z = this->state()->vp->p.z;
    Vec3 m = this->state()->vp->get_momentum();
    msg.mx = m.x;
    msg.my = m.y;
    msg.mz = m.z;
    msg.broadcast();
    #endif
    //Super::born(state);
}

} // ItemDrops
