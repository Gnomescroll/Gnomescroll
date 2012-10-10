#pragma once

#include <common/template/elastic_object_list.hpp>
#include <serializer/constants.hpp>
#include <net_lib/server.hpp>

namespace serializer
{

void init_items();
void teardown_items();

int save_player_item(class Item::Item* item, LocationNameID location_name_id, class PlayerItemSaveData* data);

void save_player_container(int client_id, int container_id, bool remove_items_after);

int begin_player_load(UserID user_id, int client_id);
bool load_player_container(int player_load_id, int container_id); 
bool end_player_load(int player_load_id);


class PlayerLoadDataList: public ElasticObjectList<class PlayerLoadData, NetServer::HARD_MAX_CONNECTIONS>
{
    public:
        const char* name() { return "PlayerLoadData"; }

    PlayerLoadDataList() :
        ElasticObjectList<class PlayerLoadData, NetServer::HARD_MAX_CONNECTIONS>(PLAYER_LOAD_DATA_LIST_HARD_MAX)
    { this->print(); }
};

class PlayerContainerLoadDataList: public ElasticObjectList<class PlayerContainerLoadData, NetServer::HARD_MAX_CONNECTIONS>
{
    public:
        const char* name() { return "PlayerContainerLoadDataList"; }

    PlayerContainerLoadDataList() :
        ElasticObjectList<class PlayerContainerLoadData, NetServer::HARD_MAX_CONNECTIONS>(PLAYER_CONTAINER_LOAD_DATA_LIST_HARD_MAX)
    { this->print(); }
};

class PlayerItemLoadDataList: public ElasticObjectList<class PlayerItemLoadData, 1024>
{
    public:
        const char* name() { return "PlayerItemLoadDataList"; }

    PlayerItemLoadDataList() :
        ElasticObjectList<class PlayerItemLoadData, 1024>(PLAYER_ITEM_LOAD_DATA_LIST_HARD_MAX)
    { this->print(); }
};

class PlayerItemSaveDataList: public ElasticObjectList<class PlayerItemSaveData, 1024>
{
    public:
        const char* name() { return "PlayerItemSaveDataList"; }

    PlayerItemSaveDataList() :
        ElasticObjectList<class PlayerItemSaveData, 1024>(PLAYER_ITEM_SAVE_DATA_LIST_HARD_MAX)
    { this->print(); }
};

}   // serializer
