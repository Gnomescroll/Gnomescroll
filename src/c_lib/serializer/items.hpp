#pragma once

#include <item/common/enum.hpp>

namespace serializer
{

extern uint32_t item_global_id;

void save_items();
SerializerError save_item(ItemID id);
SerializerError load_item(ItemID id);

SerializerError load_item_global_id();
SerializerError write_item_global_id();

uint32_t get_new_item_global_id();

}   // serializer
