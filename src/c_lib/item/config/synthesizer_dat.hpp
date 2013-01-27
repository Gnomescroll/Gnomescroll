#pragma once

namespace Item
{

//void synthesizer_item_def(const char* item_name, int cost);
//void synthesizer_item_set(int slotx, int sloty);

void load_synthesizer()
{
    synthesizer_item_def("laser_rifle", 5);
    synthesizer_item_set(0,2);

    synthesizer_item_def("block_placer", 100);
    synthesizer_item_set(4,1);

    //top row
    synthesizer_item_def("crafting_bench_basic", 1);
    synthesizer_item_set(0,0);

    synthesizer_item_def("smelter_basic", 1);
    synthesizer_item_set(1,0);

    synthesizer_item_def("cryofreezer_small", 3);
    synthesizer_item_set(2,0);

    synthesizer_item_def("crusher", 25);
    synthesizer_item_set(3,0);

    #if !PRODUCTION
    synthesizer_item_def("cryptid_larvae", 5);
    synthesizer_item_set(3,1);

    //synthesizer_item_def("crystal_seed", 5);
    //synthesizer_item_set(3,1);

    //synthesizer_item_def("crystal_seed2", 5);
    //synthesizer_item_set(3,2);

    synthesizer_item_def("acadia_seed", 5);
    synthesizer_item_set(3,2);

    synthesizer_item_def("agent_spawner", 25);
    synthesizer_item_set(4,0);

    synthesizer_item_def("energy_core", 25);
    synthesizer_item_set(4,2);

    synthesizer_item_def("blue_mining_laser", 1);
    synthesizer_item_set(0,1);

    synthesizer_item_def("red_mining_laser", 1);
    synthesizer_item_set(1,1);

    synthesizer_item_def("green_mining_laser", 1);
    synthesizer_item_set(2,1);

    synthesizer_item_def("boon_crank", 1);
    synthesizer_item_set(1,2);

    synthesizer_item_def("placer_plant_example", 1);
    synthesizer_item_set(2,2);

    #endif
}

}   // Item
