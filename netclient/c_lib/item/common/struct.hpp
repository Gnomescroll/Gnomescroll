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
    int reagant_num;
    int reagant[6];
    int reagant_count[6];

    CraftingRecipe()
    {
        init();
    }

    void init()
    {
        output = 0;
        reagant_num = 0;
        for(int i=0; i<6; i++)
        {
            reagant[i] = 0;
            reagant_count[i] = 1;
        } 
    }

};

}
