#pragma once

#include <item/item.hpp>

namespace serializer
{
namespace redis
{

void init_items();
void teardown_items();

void save_player_container(int client_id, int container_id, bool remove_items_after);

}   // redis
}   // serializer
