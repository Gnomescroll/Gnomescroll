#include "CtoS.hpp"

#if DC_CLIENT
namespace Item
{

inline void request_item_create_CtoS::handle() {}

} // Item
#endif

#if DC_SERVER
namespace Item
{

inline void request_item_create_CtoS::handle()
{
    #if PRODUCTION
    GS_ASSERT(false);
    return;
    #else
    Agents::Agent* agent = NetServer::agents[this->client_id];
    if (agent == NULL) return;

    int stack_size = (int)this->stack_size;
    int max_stack_size = get_max_stack_size((ItemType)this->type);
    if (stack_size > max_stack_size)
        stack_size = max_stack_size;

    struct Vec3 position = agent->get_center();
    Item* item = ItemParticle::create_item_particle((ItemType)this->type, position, vec3_init(0,0,0));
    IF_ASSERT(item == NULL) return;
    item->stack_size = stack_size;
    #endif
}

} // Item
#endif
