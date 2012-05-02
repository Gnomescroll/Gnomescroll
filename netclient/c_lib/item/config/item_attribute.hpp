#pragma once

namespace Item
{


class ItemAttribute
{
	public:

	int item_group_type;
	
	//IG_PLACER
	char* placer_block_type;	//type of block that it creates

	//IG_HITSCAN_WEAPON
	int hitscan_fire_cooldown;		// ms per bullet
	int hitscan_damage; 			// damage
	int hitscan_max_ammo;			// max ammo
	int hitscan_bullet_effect_enum;	// bullet effect

	//IG_MELEE_WEAPON
	int melee_fire_cooldown;
	int melee_damage;

	//IG_MINING_LASER
	int mining_fire_cooldown;
	int mining_damage;
	int mining_block_damage;

	ItemAttribute()
	{
		init(IG_ERROR);
	}

	void init(int group_type)
	{
		item_group_type = group_type;
		placer_block_type = NULL;
	}
};

}