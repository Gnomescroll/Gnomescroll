#pragma once

#include <common/template/elastic_object_list.hpp>
#include <serializer/constants.hpp>
#include <net_lib/server.hpp>

namespace serializer
{

void init_items();
void teardown_items();

int write_container_contents_string(char* buf, size_t buffer_size, const class ItemContainer::ItemContainerInterface* container);

}   // serializer
