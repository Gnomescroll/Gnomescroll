


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

/*
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
*/

#include <c_lib/item/common/enum.hpp>

#if DC_CLIENT
#include <c_lib/SDL/texture_sheet_loader.hpp>
#endif

namespace Item
{

int texture_alias(const char* spritesheet);
void item_def(int id, int group, const char* name);
void sprite_def(int spritesheet, int xpos, int ypos);
void sprite_def(int alias);
int sprite_alias(int spritesheet, int xpos, int ypos);

class ItemAttribute s;

void load_item_dat()
{
    int i0 = texture_alias("media/sprites/i00.png");
    int i1 = texture_alias("media/sprites/i01.png");

    //int error_sprite = sprite_alias(i0, 4, 1);

    item_def(0, IG_ERROR , "error_item");
    sprite_def(i0, 4,1);

    item_def(1, IG_PLACER , "regolith");
    sprite_def(i1, 2,3);

    item_def(2, IG_RESOURCE , "copper_ore");
    sprite_def(i1, 1,3);

    item_def(3, IG_RESOURCE , "copper_bar");
    sprite_def(i1, 1,2);

    item_def(4, IG_RESOURCE , "quartz_crystal");
    sprite_def(i0, 1,5);

    item_def(5, IG_HITSCAN_WEAPON , "laser_rifle");
    sprite_def(i0, 3,7);

    item_def(6, IG_MELEE_WEAPON , "copper_shovel");
    sprite_def(i1, 1,1);

    item_def(7, IG_MINING_LASER , "mining_laser");
    sprite_def(i0, 3,5);

    #if DC_CLIENT
    LUA_save_item_texture();
    #endif
}

}


namespace Item
{

int _current_item_id = 0;

void item_def(int id, int group, const char* name)
{
    _current_item_id = id;

    s.init(group);
    
    if(group_array[id] != IG_ERROR)
    {
        printf("ITEM CONFIG ERROR: item id conflict, id= %i \n", id);
        abort();
    }
    group_array[id] = group; //check

    
}

#if DC_CLIENT

int texture_alias(const char* spritesheet)
{
    return LUA_load_item_texture_sheet((char*) spritesheet);
}

void sprite_def(int spritesheet, int ypos, int xpos)
{
    ypos -= 1;
    xpos -= 1;

    if(xpos < 0 && ypos < 0)
    {
        printf("ITEM CONFIG ERROR: id= %i xpos,ypos less than zero \n", _current_item_id);
        assert(false);
    }

    int index = LUA_blit_item_texture(spritesheet, xpos, ypos);
    sprite_array[_current_item_id] = index; //check
}

void sprite_def(int alias)
{
    sprite_array[_current_item_id] = alias;
}

int sprite_alias(int spritesheet, int ypos, int xpos)
{
    ypos -= 1;
    xpos -= 1;

    if(xpos < 0 && ypos < 0)
    {
        printf("ITEM CONFIG ERROR: sprite alias xpos,ypos less than zero \n");
        assert(false);
    }
    return LUA_blit_item_texture(spritesheet, xpos, ypos);
}
#else
int texture_alias(const char* spritesheet) {return 0;}
void sprite_def(int spritesheet, int xpos, int ypos) {}
void sprite_def(int alias) {}
int sprite_alias(int spritesheet, int xpos, int ypos) { return 0; }
#endif

}
