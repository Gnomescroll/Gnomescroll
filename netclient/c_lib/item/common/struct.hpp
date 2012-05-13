#pragma once

/*
typedef enum
{
    IG_ERROR,
    IG_RESOURCE,    //does nothing, resources, stackable
    IG_PLACER,  //consumed to create block
    IG_HITSCAN_WEAPON,
    IG_MELEE_WEAPON,
    IG_MINING_LASER,

} ItemGroups;
*/


namespace Item
{

class NaniteStoreItem
{
    public:

    int item_type;
    int nanite_cost;

    int level;
    int xslot;
    int yslot;

    NaniteStoreItem()
    {
        item_type = NULL_ITEM_TYPE;
        nanite_cost = NULL_COST;
        
        level = NULL_ITEM_LEVEL;
        xslot = NULL_SLOT;
        yslot = NULL_SLOT;
    }
};

class CraftingRecipe
{
    public:

    int output; //item type
    int reagent_num;
    int reagent[CRAFT_BENCH_INPUTS_MAX];
    int reagent_count[CRAFT_BENCH_INPUTS_MAX];

    CraftingRecipe()
    {
        init();
    }

    void init()
    {
        output = NULL_ITEM_TYPE;
        reagent_num = 0;
        for(int i=0; i<CRAFT_BENCH_INPUTS_MAX; i++)
        {
            reagent[i] = NULL_ITEM_TYPE;
            reagent_count[i] = 1;
        } 
    }
};

}
