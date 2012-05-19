#pragma once

#include <item/common/struct.hpp>

namespace Item
{

//void nanite_item_def(const char* item_name, int cost);
//void nanite_item_set(int level, int slotx, int sloty);

void load_nanite_store()
{
    nanite_item_def("mining_laser", 1);
    nanite_item_set(0, 0,0);

    nanite_item_def("laser_rifle", 5);
    nanite_item_set(0, 1,0);

    nanite_item_def("crate_3", 10);
    nanite_item_set(0, 0,1);

}


}
