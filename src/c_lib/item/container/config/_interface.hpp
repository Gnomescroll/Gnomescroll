#pragma once

#include <item/container/config/types.hpp>

namespace ItemContainer
{

/* Public Attribute Accessors */

class ContainerAttributes* get_attr(ItemContainerType type);
int get_container_max_slots(ItemContainerType type);
int get_container_xdim(ItemContainerType type);
int get_container_ydim(ItemContainerType type);
int get_container_alt_max_slots(ItemContainerType type);
int get_container_alt_xdim(ItemContainerType type);
int get_container_alt_ydim(ItemContainerType type);
bool container_type_is_attached_to_agent(ItemContainerType type);
bool container_type_is_block(ItemContainerType type);

/* Configuration Loader */

void init_config();
void teardown_config();

}   // ItemContainer
