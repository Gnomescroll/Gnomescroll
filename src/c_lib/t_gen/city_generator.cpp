#include <t_gen/city_generator.hpp>
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

void init_cities()
{
    processor = t_map::get_cube_type("control_node");
    green = t_map::get_cube_type("city_green");
    red = t_map::get_cube_type("city_red");
    purple = t_map::get_cube_type("city_blue");
    gray = t_map::get_cube_type("city_gray");
    storage = t_map::get_cube_type("storage_block_small");
    cryofreezer = t_map::get_cube_type("cryofreezer_small");
    smelter = t_map::get_cube_type("smelter_basic");
    bench = t_map::get_cube_type("crafting_bench_basic");
    crusher = t_map::get_cube_type("crusher");
    steelA = t_map::get_cube_type("steel_block_1");
    steelB = t_map::get_cube_type("steel_block_2");
    steelC = t_map::get_cube_type("steel_block_3");
    glowgreen = t_map::get_cube_type("green_glow");
    glowblue = t_map::get_cube_type("blue_glow");
    battery = t_map::get_cube_type("battery2");
    stone = t_map::get_cube_type("rock");
    regolith = t_map::get_cube_type("regolith");
    temple[0] = t_map::get_cube_type("temple_runes1");
    temple[1] = t_map::get_cube_type("temple_runes2");
    temple[2] = t_map::get_cube_type("temple_runes3");
    temple[3] = t_map::get_cube_type("temple_runes4");
    temple[4] = t_map::get_cube_type("temple_runes5");
    temple[5] = t_map::get_cube_type("temple_runes6");
    temple[6] = t_map::get_cube_type("temple_runes7");
    temple[7] = t_map::get_cube_type("temple_runes8");
    temple[8] = t_map::get_cube_type("temple_runes9");
    temple[9] = t_map::get_cube_type("temple_runes10");
    temple[10] = t_map::get_cube_type("temple_runes11");
    temple[11] = t_map::get_cube_type("temple_runes12");
    temple[12] = t_map::get_cube_type("temple_runes13");
    temple[13] = t_map::get_cube_type("temple_runes14");
    temple[14] = t_map::get_cube_type("tile_floor");

    prevsubway.x = 256;
    prevsubway.y = 256;
    prevsubway.z = 32;

    BestRoad.x = 0;
    BestRoad.y = 0;
}

void generate_city()
{

        int actual_buildings = 0;
        int building_randomizer = 0;
        int x;
        int y;
        int cx;
        int cy;
        int prevx;
        int prevy;

    for (int count=1; count <= CITY_AMOUNT; count++)
    {

        prevx = cx = x = distribute_gaussian(0, map_dim.x - 1, 3);
        prevy = cy = y = distribute_gaussian(0, map_dim.y - 1, 3);
        printf("Generating alien city at %d, %d, %d \n", x, y, t_map::get_highest_open_block(x, y) - 1);

        actual_buildings = BUILDINGS_IN_CITY + randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);

        printf("Starting to generate %d buildings... \n", actual_buildings);

        for (int buildingnum=1; buildingnum <= actual_buildings; buildingnum++)
        {
            prevx = BestRoad.x;
            prevy = BestRoad.y;
            cx += randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);
            cy += randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);
            if (cx >= map_dim.x) cx = 32;
            if (cy >= map_dim.y) cy = 32;
            if (cx < 0) cx = map_dim.x - 64;
            if (cy < 0) cy = map_dim.y - 64;

                building_randomizer = randrange(1, BUILDING_AMOUNT); //1 is lab, 2 is skyscraper, 3 is subway station, 4 is house, 5 is shop, 6 is transmission tower, 7 is a square, 8 is bunker, 9 is temple
                if (building_randomizer == 1 && isGood(cx, cy, cx + LAB_ROOM_SIZE + LAB_RANDOMNESS, cy + LAB_ROOM_SIZE + LAB_RANDOMNESS))
                {
                    generate_lab(cx, cy);
                }
                if (building_randomizer == 2 && isGood(cx, cy, cx + 6, cy + 6))
                {
                    generate_skyscraper(cx, cy);
                }
                if (building_randomizer == 3 && isGood(cx, cy, cx + SUBWAY_STATION_SIZE, cy + SUBWAY_STATION_SIZE))
                {
                    generate_subway_station(cx, cy);
                }
            if (building_randomizer == 4 && isGood(cx, cy, cx + HOUSE_ROOM_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS, cy + HOUSE_ROOM_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS))
            {
                generate_house(cx, cy);
            }
            if (building_randomizer == 5 && isGood(cx, cy, cx + SHOP_SIZE, cy + SHOP_SIZE))
            {
                generate_shop(cx, cy);
                generate_column(cx, cy, get_highest_area_block(cx, cy, cx + SHOP_SIZE, cy + SHOP_SIZE) - 1, SHOP_SIZE, SHOP_SIZE);
            }
            if (building_randomizer == 6 && isGood(cx, cy, cx + 2, cy + 2))
            {
                generate_transmission_tower(cx, cy);
            }
            if (building_randomizer == 7)
            {
                printf("Leaving some empty space \n");
                cx = cx + randrange(CITY_RANDOMNESS / 2, CITY_RANDOMNESS);
                cy = cy + randrange(CITY_RANDOMNESS / 2, CITY_RANDOMNESS);
            }
            if (building_randomizer == 8 && isGood(cx, cy, cx + BUNKER_ROOM_SIZE + BUNKER_RANDOMNESS, cy + BUNKER_ROOM_SIZE + BUNKER_RANDOMNESS))
            {
                generate_bunker(cx, cy);
            }
            if (building_randomizer == 9 && isGood(cx, cy, cx + TEMPLE_SIZE, cy + TEMPLE_SIZE))
            {
                generate_column(cx, cy, get_highest_area_block(cx, cy, cx + TEMPLE_SIZE, cy + TEMPLE_SIZE) - 1, TEMPLE_SIZE, TEMPLE_SIZE);
                generate_temple(cx, cy);
            }
                find_closest_road_spot(cx, cy);
                create_road(BestRoad.x, BestRoad.y, t_map::get_highest_open_block(BestRoad.x, BestRoad.y) - 1, prevx, prevy, t_map::get_highest_open_block(prevx, prevy) - 1);
        }
    }
}

void generate_lab(int x, int y)
{
    int z = t_map::get_highest_open_block(x, y) - 1;
    printf("Generating a lab at %d, %d, %d \n", x, y, z);
    CubeType LabBlock[]={steelA, steelB, steelC};
    int CurrentSizeX;
    int CurrentSizeY;
    int PrevX = x;
    int PrevY = y;
    int PrevZ = z;
    for(int RoomsMade = 0; RoomsMade < LAB_ROOMS; RoomsMade++)
    {
        CurrentSizeX = randrange(LAB_ROOM_SIZE - LAB_RANDOMNESS, LAB_ROOM_SIZE + LAB_RANDOMNESS);
        CurrentSizeY = randrange(LAB_ROOM_SIZE - LAB_RANDOMNESS, LAB_ROOM_SIZE + LAB_RANDOMNESS);
        switch (randrange(1, 6))
        {
            case 1:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            else
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            PrevX = x;
            x += CurrentSizeX;
            x = translate_point(x);
            break;
            case 2:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            else
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            PrevY = y;
            y += CurrentSizeY;
            y = translate_point(y);
            break;
            case 3:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            else
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            PrevZ = z;
            z += LAB_ROOM_HEIGHT;
            break;
            case 4:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            else
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            PrevX = x;
            x -= CurrentSizeX;
            x = translate_point(x);
            break;
            case 5:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            else
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            PrevY = y;
            y -= CurrentSizeY;
            y = translate_point(y);
            break;
            case 6:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            else
            generate_room(LabBlock[randrange(0, sizeof(LabBlock) / sizeof(*LabBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + LAB_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), random_bool(), random_bool(), random_bool(), 1, random_bool(), random_bool(), random_bool());
            PrevZ = z;
            z -= LAB_ROOM_HEIGHT;
            break;
        }
    }
}

void generate_skyscraper(int x, int y)
{
    int z = t_map::get_highest_open_block(x, y);
    printf ("Generating a skyscraper at %d, %d, %d \n", x, y, z);
    int floors = (map_dim.z - z) / SKYSCRAPER_ROOM_HEIGHT - 1;
    CubeType SkyscraperBlock[] = {steelA, steelB, steelC, red, purple, green, gray};
    CubeType ActualBlock = SkyscraperBlock[randrange(0, sizeof(SkyscraperBlock) / sizeof(*SkyscraperBlock) - 1)];
    CubeType GlowBlock[2] = {glowblue, glowgreen};
    if(floors < 4) return;
    generate_room(ActualBlock, x, y, z, x + floors + 1, y + floors + 1, z + SKYSCRAPER_ROOM_HEIGHT, x, y + floors / 2 - 1, z, x + 1, y + floors / 2 + 1, z + 4, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, 0, 0, random_bool(), 0, 0, 0);
    for(int FloorsMade = 1; FloorsMade < floors; FloorsMade++)
    {
        generate_room(ActualBlock, x, y, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT, x + floors - FloorsMade + 2, y + floors - FloorsMade + 2, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT + SKYSCRAPER_ROOM_HEIGHT, x + 1, y + 1, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT, x + 2, y + 2, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT, x + 1, y + 1, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT + SKYSCRAPER_ROOM_HEIGHT, x + 1, y + 1, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT + SKYSCRAPER_ROOM_HEIGHT, 0, 0, 0, 0, 1, random_bool(), 0, random_bool());
        generate_area(x + 1, y + 1, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT + SKYSCRAPER_ROOM_HEIGHT / 2, x + 2, y + 2, z + FloorsMade * SKYSCRAPER_ROOM_HEIGHT + SKYSCRAPER_ROOM_HEIGHT / 2, GlowBlock[randrange(0, 1)]);
    }
    generate_column(x, y, z - 1, floors, floors);
}

void generate_subway_station(int x, int y)
{
    int z = t_map::get_highest_open_block(x, y) - SUBWAY_STATION_HEIGHT;
    printf ("Generating a subway station at %d, %d, %d \n", x, y, z);
    CubeType SubBlock[] = {steelA, steelB, steelC, red, green, gray, purple};
    generate_room(SubBlock[randrange(0, sizeof(SubBlock) / sizeof(*SubBlock) - 1)], x, y, z + SUBWAY_STATION_HEIGHT, x + SUBWAY_STATION_SIZE, y + SUBWAY_STATION_SIZE, z + SUBWAY_STATION_HEIGHT * 2, x, y + SUBWAY_STATION_SIZE / 2, z + 1 + SUBWAY_STATION_HEIGHT, x, y + SUBWAY_STATION_SIZE / 2 + 1, z + SUBWAY_STATION_HEIGHT * 2 - 1, x + SUBWAY_STATION_SIZE / 2, y, z + 1 + SUBWAY_STATION_HEIGHT, x + SUBWAY_STATION_SIZE / 2, y, z + 2 + SUBWAY_STATION_HEIGHT, 1, 0, 0, 0, 1, 0, 0, 0);
    generate_room(SubBlock[randrange(0, sizeof(SubBlock) / sizeof(*SubBlock) - 1)], x, y, z, x + SUBWAY_STATION_SIZE, y + SUBWAY_STATION_SIZE, z + SUBWAY_STATION_HEIGHT, x + SUBWAY_STATION_SIZE / 2, y + SUBWAY_STATION_SIZE / 2, z + 1, x + SUBWAY_STATION_SIZE / 2, y + SUBWAY_STATION_SIZE / 2, z + 1 + SUBWAY_STATION_HEIGHT, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, 0, 0, 0, 0, 0, 0);
    generate_tunnel(x, y, z, prevsubway.x, prevsubway.y, prevsubway.z);
    prevsubway.x = x;
    prevsubway.y = y;
    prevsubway.z = z;
}

void generate_house(int x, int y)
{
 int z = t_map::get_highest_open_block(x, y) - 1;
    printf("Generating a house at %d, %d, %d \n", x, y, z);
    CubeType HouseBlock[]={red, green, purple, gray};
    int CurrentSizeX;
    int CurrentSizeY;
    int PrevX = x;
    int PrevY = y;
    int PrevZ = z;
    for(int RoomsMade = 0; RoomsMade < HOUSE_ROOMS; RoomsMade++)
    {
        CurrentSizeX = randrange(HOUSE_ROOM_SIZE - HOUSE_RANDOMNESS, HOUSE_ROOM_SIZE + HOUSE_RANDOMNESS);
        CurrentSizeY = randrange(HOUSE_ROOM_SIZE - HOUSE_RANDOMNESS, HOUSE_ROOM_SIZE + HOUSE_RANDOMNESS);
        switch (randrange(1, 6))
        {
            case 1:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevX = x;
            x += CurrentSizeX;
            x = translate_point(x);
            break;
            case 2:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevY = y;
            y += CurrentSizeY;
            y = translate_point(y);
            break;
            case 3:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevZ = z;
            z += HOUSE_ROOM_HEIGHT;
            break;
            case 4:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevX = x;
            x -= CurrentSizeX;
            x = translate_point(x);
            break;
            case 5:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevY = y;
            y -= CurrentSizeY;
            y = translate_point(y);
            break;
            case 6:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(HouseBlock[randrange(0, sizeof(HouseBlock) / sizeof(*HouseBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + HOUSE_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, random_bool(), 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevZ = z;
            z -= HOUSE_ROOM_HEIGHT;
            break;
        }
    }
}


void generate_shop(int x, int y)
{
    int z = get_highest_area_block(x, y, x + SHOP_SIZE, y + SHOP_SIZE);
    printf ("Generating a shop at %d, %d, %d \n", x, y, z);
    CubeType ShopBlock[] = {steelA, steelB, steelC, red, green, gray, purple};
    generate_room(ShopBlock[randrange(0, sizeof(ShopBlock) / sizeof(*ShopBlock) - 1)], x, y, z, x + SHOP_SIZE, y + SHOP_SIZE, z + SHOP_HEIGHT, x, y + SHOP_SIZE / 2, z + 1, x, y + SHOP_SIZE / 2 + 1, z + SHOP_HEIGHT - 1, x + SHOP_SIZE / 2, y, z + 1, x + SHOP_SIZE / 2, y, z + 2, random_bool(), 0, 0, 0, 1, random_bool(), 0, 1);
}

void generate_transmission_tower(int x, int y)
{
    int z = t_map::get_highest_open_block(x, y);
    printf ("Generating a transmission tower at %d, %d, %d \n", x, y, z);
    CubeType TowerBlock[] = {steelA, steelB, steelC, gray};
    generate_area(x, y, z, x + 2, y + 2, map_dim.z - 2, TowerBlock[randrange(0, sizeof(TowerBlock) / sizeof(*TowerBlock) - 1)]);
    generate_area(x + 1, y + 1, z, x + 1, y + 1, map_dim.z - 2, battery);
    t_map::set(x + 1, y + 1, map_dim.z - 1, processor);
}

void create_road(int x, int y, int z, int ox, int oy, int oz)
{
    printf("Connecting %d, %d, %d with %d, %d, %d with a road...\n", x, y, z, ox, oy, oz);
    CubeType RoadBlock[] = {steelA, steelB, steelC, stone, green, red, purple, gray};
    CubeType ActualBlock = RoadBlock[randrange(0, sizeof(RoadBlock) / sizeof(*RoadBlock) - 1)];
    for(int LinesMade = 0; LinesMade < ROAD_SIZE; LinesMade++)
    {
        generate_line(x + LinesMade, y, z, ox + LinesMade, oy, oz, ActualBlock, 1);
    }
}

void generate_temple(int x, int y)
{
    int z = get_highest_area_block(x, y, x + TEMPLE_SIZE, y + TEMPLE_SIZE);
    printf ("Generating a temple at %d, %d, %d \n", x, y, z);
    CubeType FloorBlock = temple[14];
    generate_area(x, y, z, x + TEMPLE_SIZE, y + TEMPLE_SIZE, z, FloorBlock);
    for(int StepsMade=1; StepsMade <= TEMPLE_SIZE; StepsMade++)
    {
        for(int i = x + StepsMade; i <= x + TEMPLE_SIZE - StepsMade; i++)
        {
            t_map::set(i, y + StepsMade, z + StepsMade, temple[randrange(0, 13)]);
            t_map::set(i, y + TEMPLE_SIZE - StepsMade, z + StepsMade, temple[randrange(0, 13)]);
        }
        for(int j = y + StepsMade; j <= y + TEMPLE_SIZE - StepsMade; j++)
        {
            t_map::set(x + StepsMade, j, z + StepsMade, temple[randrange(0, 13)]);
            t_map::set(x + TEMPLE_SIZE - StepsMade, j, z + StepsMade, temple[randrange(0, 13)]);
        }
    }
    generate_area(x + TEMPLE_SIZE / 2 - 2, y, z + 1, x + TEMPLE_SIZE /2 + 2, y + 3, z + 4, stone);
    degenerate_area(x + TEMPLE_SIZE / 2 - 1, y, z + 1, x + TEMPLE_SIZE /2 + 1, y + 3, z + 3);
}

void generate_bunker(int x, int y)
{
    int z = t_map::get_highest_open_block(x, y);
    printf ("Generating a bunker at %d, %d, %d \n", x, y, z);
    CubeType BunkerBlock[]={steelA, steelB, steelC};
    int CurrentSizeX;
    int CurrentSizeY;
    int PrevX = x;
    int PrevY = y;
    int PrevZ = z;
    generate_sphere(vec3i_init(x + BUNKER_SPHERE_RADIUS / 2, y + BUNKER_SPHERE_RADIUS / 2, z), BUNKER_SPHERE_RADIUS, stone);
    for(int RoomsMade = 0; RoomsMade < BUNKER_ROOM_AMOUNT; RoomsMade++)
    {
        CurrentSizeX = randrange(BUNKER_ROOM_SIZE - BUNKER_RANDOMNESS, BUNKER_ROOM_SIZE + BUNKER_RANDOMNESS);
        CurrentSizeY = randrange(BUNKER_ROOM_SIZE - BUNKER_RANDOMNESS, BUNKER_ROOM_SIZE + BUNKER_RANDOMNESS);
        switch (randrange(1, 5)) //no ascent, this is a bunker!
        {
            case 1:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevX = x;
            x += CurrentSizeX;
            x = translate_point(x);
            break;
            case 2:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevY = y;
            y += CurrentSizeY;
            y = translate_point(y);
            break;
            case 3:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevZ = z;
            z -= BUNKER_ROOM_HEIGHT;
            break;
            case 4:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevX = x;
            x -= CurrentSizeX;
            x = translate_point(x);
            break;
            case 5:
            if(PrevX <= x && PrevY <= y && PrevZ <= z)
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, PrevX + 5, PrevY + 5, PrevZ + 1, x + 5, y + 5, z + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            else
            generate_room(BunkerBlock[randrange(0, sizeof(BunkerBlock) / sizeof(*BunkerBlock) - 1)], x, y, z, x + CurrentSizeX, y + CurrentSizeY, z + BUNKER_ROOM_HEIGHT, x + 5, y + 5, z + 1, PrevX + 5, PrevY + 5, PrevZ + 2, x + 1, y + 1, z + 1, x + 1, y + 1, z + 1, 0, 0, random_bool(), random_bool(), random_bool(), random_bool(), random_bool(), random_bool());
            PrevY = y;
            y -= CurrentSizeY;
            y = translate_point(y);
            break;
        }
    }
}

void generate_column(int x, int y, int z, int SizeX, int SizeY)
{
    printf ("Generating a column at %d, %d, %d \n", x, y, z);
    generate_area(x, y, 1, x + SizeX, y + SizeY, z, stone);
}

void generate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz, CubeType material)
{
    if (maxz >= map_dim.z) return;
    if (maxx >= map_dim.x) return;
    if (maxy >= map_dim.y) return;
    int special = 0;
    if (material == t_map::get_cube_type("cryofreezer_small"))
    {
        special = 1;
    }
    if (material == t_map::get_cube_type("storage_block_small"))
    {
        special = 2;
    }
    if (material == t_map::get_cube_type("smelter_basic"))
    {
        special = 3;
    }
    if (material == t_map::get_cube_type("crafting_bench_basic"))
    {
        special = 4;
    }
    if (material == t_map::get_cube_type("crusher"))
    {
        special = 5;
    }
    if (special == 0)
    {
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    t_map::set(i,j,k,material);
    }
    if (special == 1)
    {
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    create_cryofreezer(i,j,k);
    }
    if (special == 2)
    {
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    create_storage(i,j,k);
    }
    if (special == 3)
    {
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    create_smelter(i,j,k);
    }
    if (special == 4)
    {
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    create_bench(i,j,k);
    }
    if (special == 5)
    {
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    create_crusher(i,j,k);
    }
}

void degenerate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz)
{
    if (minz < 0) return;
    if (minx < 0) return;
    if (miny < 0) return;
    if (maxz >= map_dim.z) return;
    if (maxx >= map_dim.x) return;
    if (maxy >= map_dim.y) return;
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    t_map::set(i,j,k,EMPTY_CUBE);
}

void generate_tunnel(int x, int y, int z, int otherx, int othery, int otherz)
{
    printf("Generating a subway tunnel between %d, %d, %d and %d, %d, %d\n", x, y, z, otherx, othery, otherz);
    for(int LinesMade = 0; LinesMade < SUBWAY_TUNNEL_SIZE; LinesMade++)
    {
        generate_line(x + LinesMade, y, z, otherx + LinesMade, othery, otherz, steelA, 1);
        generate_line(x + LinesMade, y, z + SUBWAY_TUNNEL_SIZE, otherx + LinesMade, othery, otherz + SUBWAY_TUNNEL_SIZE, steelC, 1);
        generate_line(x, y, z + LinesMade, otherx, othery, otherz + LinesMade, steelB, 1);
        generate_line(x + SUBWAY_TUNNEL_SIZE, y, z + LinesMade, otherx + SUBWAY_TUNNEL_SIZE, othery, otherz + LinesMade, steelB, 1);
    }
    generate_line(x + SUBWAY_TUNNEL_SIZE / 2, y, z, otherx + SUBWAY_TUNNEL_SIZE / 2, othery, otherz, battery, 1);
    generate_line(x + SUBWAY_TUNNEL_SIZE / 2, y, z + SUBWAY_TUNNEL_SIZE / 2, otherx + SUBWAY_TUNNEL_SIZE / 2, othery, otherz + SUBWAY_TUNNEL_SIZE / 2, EMPTY_CUBE, 1);
}

void create_cryofreezer(int x, int y, int z)
{
    ItemContainerID id = ItemContainer::create_container_block(ItemContainer::name::cryofreezer_small, x, y, z);
    IF_ASSERT(!isValid(id)) return;
    ItemContainer::auto_add_item_to_container("methane_ice_chunk", id);
    ItemContainer::auto_add_item_to_container("methane_ice_chunk", id);
    ItemContainer::auto_add_item_to_container("methane_ice_chunk", id);
    ItemContainer::auto_add_item_to_container("methane_ice_chunk", id);
}

void create_storage(int x, int y, int z)
{
    ItemContainerID id = ItemContainer::create_container_block(ItemContainer::name::storage_block_small, x, y, z);
    IF_ASSERT(!isValid(id)) return;
    if (randrange(0, 2) == 0)
    {
        ItemContainer::auto_add_item_to_container("iron_bar", id);
        ItemContainer::auto_add_item_to_container("iridium_bar", id);
        ItemContainer::auto_add_item_to_container("gallium_bar", id);
        ItemContainer::auto_add_item_to_container("copper_bar", id);
    }
    if (randrange(0, 1) == 0)
    {
        ItemContainer::auto_add_item_to_container("coal_nugget", id);
    }
    if (randrange(0, 3) == 0)
    {
        ItemContainer::auto_add_item_to_container("green_mining_laser", id);
        ItemContainer::auto_add_item_to_container("laser_rifle", id);
    }
    if (randrange(0, 4) == 0)
    {
        ItemContainer::auto_add_item_to_container("small_charge_pack", id);
        ItemContainer::auto_add_item_to_container("energy_tank", id);
    }
}

void create_smelter(int x, int y, int z)
{
    ItemContainer::create_container_block(ItemContainer::name::smelter_basic, x, y, z);
}

void create_bench(int x, int y, int z)
{
    ItemContainer::create_container_block(ItemContainer::name::crafting_bench_basic, x, y, z);
}

void create_crusher(int x, int y, int z)
{
    ItemContainer::create_container_block(ItemContainer::name::crusher, x, y, z);
}

bool isGood(int x, int y, int maxx, int maxy)
{
    printf("Testing if position %d, %d to %d, %d is good for a building... \n", x, y, maxx, maxy);
    GS_ASSERT(x < maxx);
    GS_ASSERT(y < maxy);
    if (maxx >= map_dim.x || maxy >= map_dim.y || x < 0 || y < 0) return 0;
    int maxlevel=0;
    int minlevel=map_dim.z - 1;
    for (int i = x; i <= maxx; i++)
    for (int j = y; j <= maxy; j++)
    {
        if (isArtificial(i, j))
        {
            printf("Block is artificial! \n");
            return 0;
        }
        if (t_map::get_highest_open_block(i, j) > maxlevel) maxlevel = t_map::get_highest_open_block(i, j);
        if (t_map::get_highest_open_block(i, j) < minlevel) minlevel = t_map::get_highest_open_block(i, j);
    }
    printf("minlevel = %d \nmaxlevel = %d \n", minlevel, maxlevel);
    if (minlevel + MAXIMAL_HEIGHT_DIFFERENCE < maxlevel) return 0;
    if (maxlevel > MAXIMAL_HEIGHT) return 0;
    return 1;
}

int get_highest_area_block(int x, int y, int maxx, int maxy)
{
    int maxh=0;
    GS_ASSERT(x >= 0 && x < map_dim.x);
    GS_ASSERT(y >= 0 && y < map_dim.y);
    GS_ASSERT(maxx >= 0 && maxx < map_dim.x);
    GS_ASSERT(maxy >= 0 && maxy < map_dim.y);
    GS_ASSERT(maxx >= x);
    GS_ASSERT(maxy >= y);
    for (int i = x; i <= maxx; i++)
    for (int j = y; j <= maxy; j++)
    {
        if (t_map::get_highest_open_block(i, j) - 1 > maxh) maxh = t_map::get_highest_open_block(i, j) - 1;
    }
    return maxh;
}

void generate_room(CubeType material, int minx, int miny, int minz, int maxx, int maxy, int maxz, int minentrancex, int minentrancey, int minentrancez, int maxentrancex, int maxentrancey, int maxentracez, int minexitx, int minexity, int minexitz, int maxexitx, int maxexity, int maxexitz, bool windows, bool electric, bool crafting, bool furnace, bool computer, bool fridge, bool recycler, bool chest)
{
    generate_area(minx, miny, minz, maxx, maxy, maxz, material);
    degenerate_area(minx + 1, miny + 1, minz + 1, maxx - 1, maxy - 1, maxz - 1);
    degenerate_area(minentrancex, minentrancey, minentrancez, maxentrancex, maxentrancey, maxentracez);
    degenerate_area(minexitx, minexity, minexitz, maxexitx, maxexity, maxexitz);
    if(windows) //just holes in the walls every 3 blocks
    {
        for(int cx = minx + 1; cx < maxx; cx += 3) degenerate_area(cx, miny, minz + 2, cx, maxy, minz + 2);
        for(int cy = miny + 1; cy < maxy; cy += 3) degenerate_area(minx, cy, minz + 2, maxx, cy, minz + 2);
    }
    if(electric) generate_area((minx + maxx) / 2 - 1, (miny + maxy) / 2 - 1, minz + 1, (minx + maxx) / 2 + 1, (miny + maxy) / 2 + 1, minz + 3, battery);//make a battery pile in the middle of the room, for labs
    if(crafting) create_bench(minx + 1, miny + 1, minz + 1);
    if(furnace) create_smelter(minx + 1, maxy - 1, minz + 1);
    if(computer) t_map::set(maxx - 1, miny + 1, minz + 1, processor);
    if(fridge) create_cryofreezer(maxx - 1, maxy - 1, minz + 1);
    if(recycler) create_crusher(maxx - 2, maxy + 1, minz + 1);
    if(chest) generate_area(minx + randrange(2, 3), miny + 1, minz + 1, maxx - randrange(2, 3), miny + 1, maxz - randrange(1, 3), storage); //used mainly for shops, so large amounts needed
}

void find_closest_road_spot(int x, int y)
{
    for(int i = x - CITY_RANDOMNESS; i < x + CITY_RANDOMNESS; i++)
    for(int j = y - CITY_RANDOMNESS; j < y + CITY_RANDOMNESS; j++)
    {
        if(isArtificial(i, j) == false && sqrtf(powf(i - x, 2) + powf(j - y, 2) + powf(t_map::get_highest_open_block(i, j) - t_map::get_highest_open_block(x, y), 2)) < sqrtf(powf(BestRoad.x - x, 2) + powf(BestRoad.y - y, 2) + powf(t_map::get_highest_open_block(BestRoad.x, BestRoad.y) - t_map::get_highest_open_block(x, y), 2)))
        {
            BestRoad.x = i;
            BestRoad.y = j;
        }
    }
    BestRoad.x = translate_point(BestRoad.x);
    BestRoad.y = translate_point(BestRoad.y);
}

bool isArtificial(int x, int y)
{
    if (t_map::get(x, y, t_map::get_highest_open_block(x, y) - 1) != stone && t_map::get(x, y, t_map::get_highest_open_block(x, y) - 1) != regolith)
    return true;
    else return false;
}
}   // t_gen
