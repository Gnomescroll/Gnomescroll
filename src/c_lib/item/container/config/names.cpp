#include "names.hpp"

namespace ItemContainer
{

#define UNPACK_NAME(NAME) \
    NAME = get_type(#NAME);\
    GS_ASSERT(isValid(NAME));

namespace name
{
    
ItemContainerType hand = NULL_CONTAINER_TYPE;
ItemContainerType inventory = NULL_CONTAINER_TYPE;
ItemContainerType toolbelt = NULL_CONTAINER_TYPE;
ItemContainerType synthesizer = NULL_CONTAINER_TYPE;
ItemContainerType energy_tanks = NULL_CONTAINER_TYPE;

ItemContainerType storage_block_small = NULL_CONTAINER_TYPE;
ItemContainerType crafting_bench_basic = NULL_CONTAINER_TYPE;
ItemContainerType cryofreezer_small = NULL_CONTAINER_TYPE;
ItemContainerType smelter_basic = NULL_CONTAINER_TYPE;
ItemContainerType crusher = NULL_CONTAINER_TYPE;

void unpack()
{
    //hand = get_type("hand");
    //inventory = get_type("inventory");
    //toolbelt = get_type("toolbelt");
    //synthesizer = get_type("synthesizer");
    //energy_tanks = get_type("energy_tanks");
    
    //storage_block_small = get_type("storage_block_small");
    //crafting_bench_basic = get_type("crafting_bench_basic");
    //cryofreezer_small = get_type("cryofreezer_small");
    //smelter_basic = get_type("smelter_basic");
    //crusher = get_type("crusher");

    UNPACK_NAME(hand);
    UNPACK_NAME(inventory);
    UNPACK_NAME(toolbelt);
    UNPACK_NAME(synthesizer);
    UNPACK_NAME(energy_tanks);
    
    UNPACK_NAME(storage_block_small);
    UNPACK_NAME(crafting_bench_basic);
    UNPACK_NAME(cryofreezer_small);
    UNPACK_NAME(smelter_basic);
    UNPACK_NAME(crusher);
}

}   // name

#undef UNPACK_NAME

} // ItemContainer
