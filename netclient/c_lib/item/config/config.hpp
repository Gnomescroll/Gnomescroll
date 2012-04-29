


/*
typedef enum
{
    IG_ERROR,
    IG_RESOURCE, 	//does nothing, resources, stackable
    IG_PLACER, 	//consumed to create block
    IG_HITSCAN_WEAPON,
    IG_MELEE_WEAPON,
    IG_MINING_LASER,

} ItemGroups;
*/

#include <c_lib/item/data/enum.hpp>

#ifdef DC_CLIENT
#include <c_lib/SDL/texture_sheet_loader.hpp>
#endif
namespace Item
{

int _current_item_id = 0;

int texture_alias(const char* spritesheet)
{
#ifdef DC_CLIENT
	return LUA_load_item_texture_sheet((char*) spritesheet);
#else
	return 0;
#endif
}

void item_def(int id, int group, const char* name)
{
	_current_item_id = id;

	if(group_array[id] != IG_ERROR)
	{
		printf("ITEM DAT ERROR: item id conflict, id= %i \n", id);
		abort();
	}

	group_array[id] = group; //check
}

void sprite_def(int spritesheet, int xpos, int ypos)
{
#ifdef DC_CLIENT
	int index = LUA_blit_item_texture(spritesheet, xpos, ypos);
	sprite_array[_current_item_id] = index; //check
#endif
}

void load_item_dat()
{
	int t00 = texture_alias("media/sprites/i00.png");

	item_def(0, IG_RESOURCE , "TEST_RESOURCE1");
	sprite_def(t00, 0,0);

	item_def(1, IG_RESOURCE , "TEST_RESOURCE2");
	sprite_def(t00, 1,0);

	item_def(2, IG_RESOURCE , "TEST_RESOURCE3");
	sprite_def(t00, 0,2);

	LUA_save_item_texture();
}

}