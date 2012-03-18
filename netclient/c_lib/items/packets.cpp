#include "packets.hpp"

namespace ItemDrops
{

#if DC_CLIENT
inline void item_create_StoC::handle()
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            ClientState::grenade_refill_list->create(id, x,y,z, mx,my,mz);
            break;
            
        case OBJ_TYPE_LASER_REFILL:
            ClientState::laser_refill_list->create(id, x,y,z, mx,my,mz);
            break;
            
        default: return;
    }
}

inline void item_destroy_StoC::handle()
{
    // if (id == )
    Sound::pickup_item();
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            ClientState::grenade_refill_list->destroy(id);
            break;

        case OBJ_TYPE_LASER_REFILL:
            ClientState::laser_refill_list->destroy(id);
            break;

        default: return;
    }
}
#endif

#if DC_SERVER
inline void item_create_StoC::handle() {}
inline void item_destroy_StoC::handle() {}
#endif

}   // ItemDrops
