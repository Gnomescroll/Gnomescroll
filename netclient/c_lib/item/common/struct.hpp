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

	int item_id;
	int nanite_cost;

	int level;
	int xslot;
	int yslot;

	NaniteStoreItem()
	{
		item_id = 0;
		nanite_cost = 0;
		
		level =-1;
		xslot =-1;
		yslot =-1;
	}
};

}