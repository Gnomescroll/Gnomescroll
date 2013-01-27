#include "names.hpp"

namespace ItemContainer
{

#define UNPACK_NAME(NAME) \
    NAME = get_type(#NAME); \
    IF_ASSERT(!isValid(NAME)) all_valid = false;

namespace name
{
    
ItemContainerType hand = NULL_CONTAINER_TYPE;
ItemContainerType inventory = NULL_CONTAINER_TYPE;
ItemContainerType toolbelt = NULL_CONTAINER_TYPE;
ItemContainerType synthesizer = NULL_CONTAINER_TYPE;
ItemContainerType energy_tanks = NULL_CONTAINER_TYPE;
ItemContainerType premium_cache = NULL_CONTAINER_TYPE;
ItemContainerType equipment = NULL_CONTAINER_TYPE;

ItemContainerType storage_block_small = NULL_CONTAINER_TYPE;
ItemContainerType crafting_bench_basic = NULL_CONTAINER_TYPE;
ItemContainerType cryofreezer_small = NULL_CONTAINER_TYPE;
ItemContainerType smelter_basic = NULL_CONTAINER_TYPE;
ItemContainerType crusher = NULL_CONTAINER_TYPE;

void unpack()
{
    bool all_valid = true;
    
    UNPACK_NAME(hand);
    UNPACK_NAME(inventory);
    UNPACK_NAME(toolbelt);
    UNPACK_NAME(synthesizer);
    UNPACK_NAME(energy_tanks);
    UNPACK_NAME(premium_cache);
    UNPACK_NAME(equipment);
    
    UNPACK_NAME(storage_block_small);
    UNPACK_NAME(crafting_bench_basic);
    UNPACK_NAME(cryofreezer_small);
    UNPACK_NAME(smelter_basic);
    UNPACK_NAME(crusher);

    GS_ASSERT_ABORT(all_valid);
}

}   // name

#undef UNPACK_NAME

} // ItemContainer
