#include "free_item.hpp"

#include <c_lib/t_item/net.hpp>

namespace t_item
{


void Free_item::die()
{
#ifdef DC_SERVER
    class t_item_destroy_StoC msg;
    msg.id = this->id;
    msg.type = this->type;
    msg.broadcast();
#endif
}


}
