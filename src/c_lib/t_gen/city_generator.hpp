/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <item/container/_interface.hpp>
#include <t_map/_interface.hpp>
#include <t_map/config/block_dat.hpp>
#include <t_gen/shapes.hpp>

namespace t_gen
{

    const int CITY_RANDOMNESS = 32; //the maximal amount of buildings by which to change buildings_in_city, also used for other stuff
    const int CITY_AMOUNT = 1;
    const int BUILDINGS_IN_CITY = 64;
    const int LAB_ROOM_SIZE = 10;
    const int LAB_ROOMS=8;
    const int LAB_ROOM_HEIGHT = 12;
    const int LAB_RANDOMNESS = 4;
    const int SKYSCRAPER_ROOM_HEIGHT = 6; //size is calculated based on avaliable height, so that skyscrapers are as high as can be & keep proportions
    const int SUBWAY_STATION_SIZE = 8;
    const int SUBWAY_STATION_HEIGHT = 6;
    const int SUBWAY_TUNNEL_SIZE = 5;
    const int BUNKER_ROOM_SIZE = 6;
    const int BUNKER_ROOM_HEIGHT = 4;
    const int BUNKER_ROOM_AMOUNT = 8;
    const int BUNKER_RANDOMNESS = 4;
    const int BUNKER_SPHERE_RADIUS = BUNKER_ROOM_SIZE * BUNKER_ROOM_AMOUNT / 2;
    const int TEMPLE_SIZE = 32;
    const int HOUSE_ROOM_SIZE = 8;
    const int HOUSE_RANDOMNESS = 3;
    const int HOUSE_ROOMS = 10;
    const int HOUSE_ROOM_HEIGHT = 12;
    const int HOUSE_GARDEN = 6;
    const int SHOP_SIZE = 8;
    const int SHOP_HEIGHT = 5;
    const int ROAD_SIZE = 4;
    const int BUILDING_AMOUNT = 9;
    const int MAXIMAL_HEIGHT = map_dim.z - 1 - BUNKER_SPHERE_RADIUS; //for isGood
    const int MINIMAL_HEIGHT = BUNKER_SPHERE_RADIUS;
    const int MAXIMAL_HEIGHT_DIFFERENCE = 64;

        struct previous{int x; int y; int z;}
        prevsubway;

        struct best{int x; int y;}
        BestRoad;


    CubeType processor;
    CubeType green;
    CubeType red;
    CubeType purple;
    CubeType gray;
    CubeType storage;
    CubeType cryofreezer;
    CubeType smelter;
    CubeType bench;
    CubeType crusher;
    CubeType steelA;
    CubeType steelB;
    CubeType steelC;
    CubeType glowgreen;
    CubeType glowblue;
    CubeType battery;
    CubeType stone;
    CubeType regolith;
    CubeType temple[15];

void generate_city();
void generate_lab(int x, int y);
void generate_skyscraper(int x, int y);
void generate_subway_station(int x, int y);
void generate_house(int x, int y);
void generate_shop(int x, int y);
void generate_transmission_tower(int x, int y);
void degenerate_space(int x, int y, int z, int size);
void create_road(int x, int y, int z, int ox, int oy, int oz);
void generate_temple(int x, int y);
void generate_bunker(int x, int y);
void generate_column(int x, int y, int z, int SizeX, int SizeY);
void generate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz, CubeType material);
void degenerate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz);
void generate_tunnel(int x, int y, int z, int otherx, int othery, int otherz);
void create_cryofreezer(int x, int y, int z);
void create_storage(int x, int y, int z);
void create_smelter(int x, int y, int z);
void create_bench(int x, int y, int z);
void create_crusher(int x, int y, int z);
bool isGood(int x, int y, int maxx, int maxy);
int get_highest_area_block(int x, int y, int maxx, int maxy);
void generate_room(CubeType material, int minx, int miny, int minz, int maxx, int maxy, int maxz, int minentrancex, int minentrancey, int minentrancez, int maxentrancex, int maxentrancey, int maxentracez, int minexitx, int minexity, int minexitz, int maxexitx, int maxexity, int maxexitz, bool windows, bool electric, bool crafting, bool furnace, bool computer, bool fridge, bool recycler, bool chest);
void init_cities(void);
void find_closest_road_spot(int x, int y);
bool isArtificial(int x, int y);
int clamp_point(int point);

}   // t_gen
