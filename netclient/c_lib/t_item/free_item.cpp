#include "free_item.hpp"

#include <c_lib/t_item/net/StoC.hpp>

namespace t_item
{


void Free_item::die()
{
    #if DC_SERVER
    class free_item_destroy_StoC msg;
    msg.id = this->id;
    msg.type = this->type;
    msg.broadcast();
    #endif
}


}
