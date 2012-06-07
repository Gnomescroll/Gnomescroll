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

    nanite_item_def("crate_3", 1);
    nanite_item_set(0, 0,1);

    nanite_item_def("crate_2", 5);
    nanite_item_set(0, 1,1);

    nanite_item_def("cryofreezer_1", 3);
    nanite_item_set(0, 1,2);

    nanite_item_def("repair_kit", 3);
    nanite_item_set(0, 0,2);

    nanite_item_def("block_placer", 20);
    nanite_item_set(0, 0,3);
}


}
