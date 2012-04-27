#pragma once



namespace item
{

void init();
void teardown();

}

/*
CLIENT
*/
#if DC_CLIENT

namespace item
{



}

#endif 



/*
SERVER
*/

#if DC_SERVER

namespace item
{

void check_item_pickups();

void create_item(int inventory_id);
}
#endif


