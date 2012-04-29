#include "item.hpp"

#if DC_SERVER
#include <item/server.hpp>
#endif

namespace Item
{

#if DC_SERVER
Item::~Item()
{
    broadcast_item_destroy(this->id);
}
#endif

}
