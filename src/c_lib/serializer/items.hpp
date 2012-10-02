#pragma once

#include <item/common/enum.hpp>

namespace serializer
{

extern int64_t item_global_id;

void save_items();
SerializerError save_item(ItemID id);
SerializerError load_item(ItemID id);

SerializerError load_item_global_id();
SerializerError write_item_global_id();

int64_t get_new_item_global_id();

void init_item_serializer();
void teardown_item_serializer();

}   // serializer
