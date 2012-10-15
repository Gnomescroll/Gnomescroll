#pragma once

#include <common/template/elastic_object_list.hpp>
#include <serializer/constants.hpp>
#include <net_lib/server.hpp>

namespace serializer
{

int write_container_contents_string(char* buf, size_t buffer_size, const class ItemContainer::ItemContainerInterface* container);

// WARNING: modifies char* str
void parse_item_string(char* str, const size_t length, class ParsedItemData* data);

}   // serializer
