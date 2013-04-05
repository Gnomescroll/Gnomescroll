#include <t_gen/city_generator.hpp>
#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <item/container/_interface.hpp>
#include <t_map/_interface.hpp>
#include <t_map/config/block_dat.hpp>

namespace t_gen
{

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
        subwaycounter = 1;

        prevx = cx = x = distribute_gaussian(0, t_map::map_dim.x - 1, 3);
        prevy = cy = y = distribute_gaussian(0, t_map::map_dim.y - 1, 3);
        printf("Generating alien city at %d, %d, %d \n", x, y, t_map::get_highest_open_block(x, y) - 1);

        actual_buildings = BUILDINGS_IN_CITY + randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);

        printf("Starting to generate %d buildings... \n", actual_buildings);

        for (int buildingnum=1; buildingnum <= actual_buildings; buildingnum++)
        {
            prevx = cx;
            prevy = cy;
            cx += randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);
            cy += randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);
            if (cx >= t_map::map_dim.x) cx = 32;
            if (cy >= t_map::map_dim.y) cy = 32;
            if (cx < 0) cx = t_map::map_dim.x - 64;
            if (cy < 0) cy = t_map::map_dim.y - 64;

                create_roads(ROAD_SIZE, steelC, prevx + 38, prevy + 38, cx - 6, cy - 6, rock, regolith);
                building_randomizer = randrange(1, BUILDING_AMOUNT); //1 is lab, 2 is skyscraper, 3 is subway station, 4 is house, 5 is shop, 6 is transmission tower, 7 is a square, 8 is bunker, 9 is temple
                if (building_randomizer == 1 && isGood(cx, cy, cx + LAB_SIZE + LAB_RANDOMNESS, cy + LAB_SIZE + LAB_RANDOMNESS, rock, regolith))
                {
                    generate_lab(cx, cy, get_highest_area_block(cx, cy, cx + LAB_SIZE + LAB_RANDOMNESS, cy + LAB_SIZE + LAB_RANDOMNESS), LAB_SIZE, LAB_HEIGHT, LAB_FLOORS, LAB_RANDOMNESS, LAB_DOOR_PROBABILITY, computer, steelA, steelB, steelC, battery, smelter, cryofreezer, bench, crusher, storage);
                    generate_column(cx, cy, get_highest_area_block(cx, cy, cx + LAB_SIZE + LAB_RANDOMNESS, cy + LAB_SIZE + LAB_RANDOMNESS) - 1, LAB_SIZE + LAB_RANDOMNESS, rock);
                }
                if (building_randomizer == 2 && isGood(cx, cy, cx + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS, cy + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS, rock, regolith))
                {
                    generate_skyscraper(cx, cy, get_highest_area_block(cx, cy, cx + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS, cy + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS), SKYSCRAPER_SIZE, t_map::map_dim.z - t_map::get_highest_open_block(cx, cy) - 1, SKYSCRAPER_FLOORS, SKYSCRAPER_RANDOMNESS, SKYSCRAPER_PARTITIONS, computer, purple, green, red, cryofreezer, battery);
                    generate_column(cx, cy, get_highest_area_block(cx, cy, cx + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS, cy + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS) - 1, SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS, rock);
                }
                if (building_randomizer == 3 && isGood(cx, cy, cx + SUBWAY_STATION_SIZE, cy + SUBWAY_STATION_SIZE, rock, regolith))
                {
                    else laststation = 0;
                    generate_column(cx, cy, get_highest_area_block(cx, cy, cx + SUBWAY_STATION_SIZE, cy + SUBWAY_STATION_SIZE) - SUBWAY_STATION_HEIGHT - 1, SUBWAY_STATION_SIZE, rock);
                    generate_subway_station(cx, cy, get_highest_area_block(cx, cy, cx + SUBWAY_STATION_SIZE, cy + SUBWAY_STATION_SIZE), prevsubwayx, prevsubwayy, firstsubwayx, firstsubwayy, laststation, SUBWAY_STATION_SIZE, SUBWAY_STATION_HEIGHT, SUBWAY_TUNNEL_SIZE, gray, steelA, steelB, steelC, battery, rock);
                    prevsubwayx = cx;
                    prevsubwayy = cy;
                }
            if (building_randomizer == 4 && isGood(cx, cy, cx + HOUSE_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS, cy + HOUSE_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS, rock, regolith))
            {
                generate_house(cx, cy, get_highest_area_block(cx, cy, cx + HOUSE_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS, cy + HOUSE_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS), HOUSE_SIZE, HOUSE_HEIGHT, HOUSE_PARTITION_PROBABILITY, HOUSE_RANDOMNESS, HOUSE_GARDEN, HOUSE_GARAGE_SIZE, computer, green, red, purple, storage, cryofreezer, smelter, bench, crusher, regolith, steelA);
                generate_column(cx, cy, get_highest_area_block(cx, cy, cx + HOUSE_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS, cy + HOUSE_SIZE + HOUSE_GARDEN * 2 + HOUSE_RANDOMNESS) - 1, HOUSE_SIZE + HOUSE_RANDOMNESS, rock);
            }
            if (building_randomizer == 5 && isGood(cx, cy, cx + SHOP_SIZE, cy + SHOP_SIZE, rock, regolith))
            {
                generate_shop(cx, cy, get_highest_area_block(cx, cy, cx + SHOP_SIZE, cy + SHOP_SIZE), SHOP_SIZE, SHOP_HEIGHT, SHOP_GOODS_AMOUNT, steelA, steelB, steelC, computer, storage, cryofreezer);
                generate_column(cx, cy, get_highest_area_block(cx, cy, cx + SHOP_SIZE, cy + SHOP_SIZE) - 1, SHOP_SIZE, rock);
            }
            if (building_randomizer == 6 && isGood(cx, cy, cx + 2, cy + 2, rock, regolith))
            {
                generate_transmission_tower(cx, cy, get_highest_area_block(cx, cy, cx + 2, cy + 2), TRANSMISSION_TOWER_HEIGHT, steelA, steelB, steelC, gray, battery, computer);
            }
            if (building_randomizer == 7)
            {
                printf("Leaving some empty space \n");
                cx = cx + randrange(CITY_RANDOMNESS / 2, CITY_RANDOMNESS);
                cy = cy + randrange(CITY_RANDOMNESS / 2, CITY_RANDOMNESS);
            }
            if (building_randomizer == 8 && isGood(cx, cy, cx + BUNKER_SIZE + BUNKER_RANDOMNESS, cy + BUNKER_SIZE + BUNKER_RANDOMNESS, rock, regolith))
            {
                generate_column(cx, cy, get_highest_area_block(cx, cy, cx + BUNKER_SIZE + BUNKER_RANDOMNESS, cy + BUNKER_SIZE + BUNKER_RANDOMNESS), BUNKER_SIZE, rock);
                generate_bunker(cx, cy, get_highest_area_block(cx, cy, cx + BUNKER_SIZE + BUNKER_RANDOMNESS, cy + BUNKER_SIZE + BUNKER_RANDOMNESS), BUNKER_SIZE, BUNKER_DEPTH, BUNKER_FLOORS, BUNKER_PARTITION_PROBABILITY, BUNKER_RANDOMNESS, gray, computer, storage, cryofreezer);
            }
            if (building_randomizer == 9 && isGood(cx, cy, cx + TEMPLE_SIZE, cy + TEMPLE_SIZE, rock, regolith))
            {
                generate_column(cx, cy, get_highest_area_block(cx, cy, cx + TEMPLE_SIZE, cy + TEMPLE_SIZE) - 1, TEMPLE_SIZE, rock);
                generate_temple(cx, cy, get_highest_area_block(cx, cy, cx + TEMPLE_SIZE, cy + TEMPLE_SIZE), TEMPLE_SIZE, glowgreen, glowblue, rock);
            }
        }
    }
}

void generate_lab(int x, int y, int z, int size, int height, int floors, int randomness, int door_probability, CubeType computer, CubeType steelA, CubeType steelB, CubeType steelC, CubeType battery, CubeType smelter, CubeType cryofreezer, CubeType bench, CubeType crusher, CubeType storage)
{
    printf("Generating a lab at %d, %d, %d \n", x, y, z);
    int maxx = x + randrange(randomness * -1, randomness) + size;
    int maxy = y + randrange(randomness * -1, randomness) + size;
    int maxz = z + height + randrange(randomness * -1, randomness);
    int cx = x + 1;
    int cy = y + 1;
    int door;
    int floorcount = 1;
    generate_area(x, y, z, maxx, maxy, z, steelA); //generate the floor
    generate_area(x, y, z + 1, maxx, y, maxz, steelB); //generate a wall
    generate_area(x, y, z + 1, x, maxy, maxz, steelB);
    generate_area(maxx, y, z + 1, maxx, maxy, maxz, steelB);
    generate_area(x, maxy, z + 1, maxx, maxy, maxz, steelB);
    generate_area(x + 1, y + 1, maxz, maxx - 1, maxy - 1, maxz, steelC); //generate the roof
    while (cx < maxx)
    {
        door = randrange(1, 100);
        if (door <= door_probability)
        {
            generate_area(cx, y + 1, z + 1, cx, maxy - 1, maxz - 1, steelA);
            degenerate_area(cx, y + 1, z + 1, cx, y + (maxy - y) / 2, maxz - 1);
        }
        cx++;
    }
    while (cy < maxy)
    {
        door = randrange(1, 100);
        if (door <= door_probability)
        {
            generate_area(x + 1, cy, z + 1, x, cy, maxz - 1, steelA);
            degenerate_area(x + 1, cy, z + 1, x + (maxx - x) / 2, cy, maxz - 1);
        }
        cy++;
    }
    cx = x + 1;
    cy = y + 1;
    while (floorcount <= floors - 1)
    {
    generate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * floorcount, maxx - 1, maxy - 1, z + (maxz - z) / (floors - 1) * floorcount, steelB); //generate the floors above ground
    degenerate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * floorcount, x + randomness, y + randomness, z + (maxz - z) / (floors - 1) * floorcount);
    floorcount++;
    }
    generate_area(x + randrange(1, 2), y + 1, z + 1, maxx - randrange(1, 2), y + 1, z + randrange(1, 3), computer);
    generate_area(x + 1, y + randrange(1, 2), z + 1, x + 1, maxy - randrange(1, 2), z + randrange(1, 3), bench);
    generate_area(x + randrange(1, 2), y + 1, maxz - randrange (1, 3), maxx - randrange(1, 3), y + 1, maxz - 1, smelter);
    generate_area(x + 1, y + randrange(1, 2), maxz - randrange (1, 3), x + 1, maxy - randrange(1, 2), maxz - 1, cryofreezer);
    generate_area(maxx - 1, y + randrange(1, 2), maxz - randrange (1, 3), maxx - 1, maxy - randrange(1, 2), maxz - 1, crusher);
    generate_area(x + randrange(1, 2), maxy - 1, maxz - randrange (1, 3), maxx - randrange(1, 3), maxy - 1, maxz - 1, storage);
    generate_area((x + maxx) / 2 + randrange (randomness * -1, randomness), (y + maxy) / 2 + randrange (randomness * -1, randomness), z + 1, (x + maxx) / 2 + randomness, (y + maxy) / 2 + randomness, z + 3, battery); //create a Van Der Graaf generator near the middle
    degenerate_area(x + 1, y, z + 1, maxx - 1, y, z + 3); //generate 2 doors outside
    degenerate_area(x, y + 1, z + 1, x, maxy - 1, z + 3);
}

void generate_skyscraper(int x, int y, int z, int size, int height, int floors, int randomness, int partitions, CubeType computer, CubeType purple, CubeType green, CubeType red, CubeType cryofreezer, CubeType battery)
{
    printf ("Generating a skyscraper at %d, %d, %d \n", x, y, z);
    int maxx = x + randrange(randomness * -1, randomness) + size;
    int maxy = y + randrange(randomness * -1, randomness) + size;
    int maxz = z + height + randrange(randomness * -1, randomness);
    maxz = GS_MIN(maxz, t_map::map_dim.z-1);

    generate_area(x, y, z, maxx, maxy, z, red); //generate the floor
    generate_area(x, y, z + 1, maxx, y, maxz - 1, purple); //make walls
    generate_area(x, y, z + 1, x, maxy, maxz - 1, purple);
    generate_area(maxx, y, z + 1, maxx, maxy, maxz - 1, purple);
    generate_area(x, maxy, z + 1, maxx, maxy, maxz - 1, purple);
    generate_area(x, y, maxz, maxx, maxy, maxz, green); //generate the roof

    for (int count=1; count<=partitions; count++)
    {
        generate_area(x + (x + maxx) / 2 / partitions * count, y + 1, z + 1, x + (x + maxx) / 2 / partitions * count, maxy - 1, maxz - 1, green); //generate partitions
        degenerate_area(x + (x + maxx) / 2 / partitions * count, y + randomness, z + 1, x + (x + maxx) / 2 / partitions * count, maxy - randomness, maxz - 1); //make entrances to rooms
        count++;
    }

    for (int count=1; count<=partitions; count++)
    {
        generate_area(x + 1, y + (y + maxy) / 2 / partitions * count, z + 1, maxx - 1, y + (y + maxy) / 2 / partitions * count, maxz - 1, green); //generate partitions on y axis
        degenerate_area(x + randomness, y + (y + maxy) / 2 / partitions * count, z + 1, maxx - randomness, y + (y + maxy) / 2 / partitions * count, maxz - 1); //make entrances to rooms
    }

    for (int count=1; count<floors; count++)
    {
        generate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * count, maxx - 1, maxy - 1, z + (maxz - z) / (floors - 1) * count, green); //generate the floors above ground
        degenerate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * count, x + randomness, y + randomness, z + (maxz - z) / (floors - 1) * count); //degenerate entrances to upper floors
    }

    for (int count=1; count<floors; count++)
    {
        int cz = (z + (maxz - z) / (floors - 1)) * count + 1;
        cz = GS_MIN(maxz, t_map::map_dim.z-2);
        t_map::set(x + 2, y + 2, cz, red); //generate computer desks on each floor
        t_map::set(x + 2, y + 2, cz+1, computer); //generate computers on the desks
    }

    for (int count=1; count<floors; count++)
    {
        int cz = (z + (maxz - z) / (floors - 1)) * count + 1;
        cz = GS_MIN(maxz, t_map::map_dim.z-2);
        t_map::set(x + 8, y + 8, cz, battery); //generate a battery for "powering" the fridge above
        generate_area(x + 8, y + 8, cz+1, x + 8, y + 8, cz+1, cryofreezer); //generate the "fridge"-a cryofreezer
    }

    degenerate_area(x + randomness, y, z + 1, maxx - randomness, y, z + 3); //create an exit
}

void generate_subway_station(int x, int y, int z, int prevsubwayx, int prevsubwayy, int firstsubwayx, int firstsubwayy, bool laststation, int size, int height, int tunnel_size, CubeType gray, CubeType steelA, CubeType steelB, CubeType steelC, CubeType battery, CubeType rock)
{
    printf ("Generating a subway station at %d, %d, %d \n", x, y, z);
    int maxx = x + size;
    int maxy = y + size;
    int minz = z - height;
    generate_area(x, y, minz, maxx, y, z + height, gray); //generate walls
    generate_area(x, y, minz, x, maxy, z + height, gray);
    generate_area(maxx, y, minz, maxx, maxy, z + height, gray);
    generate_area(x, maxy, minz, maxx, maxy, z + height, gray);
    generate_area(x, y, minz, maxx, maxy, minz, steelC); //generate the floor
    generate_area(x, y, z + height, maxx, maxy, z + height, steelA);
    generate_tunnel(x + size / 2, y + size / 2, z - height / 2, prevsubwayx, prevsubwayy, tunnel_size, steelA, steelB, battery, rock);
    if (laststation) generate_tunnel(x + size / 2, y + size / 2, z - height / 2, firstsubwayx, firstsubwayy, tunnel_size, steelA, steelB, battery, rock);
    degenerate_area(x + 1, y + 1, minz + 1, maxx - 1, maxy - 1, z - 1);
    degenerate_area(x + 2, y + 2, z, maxx - 2, maxy - 2, z); //make an opening for an entrance
    degenerate_area(x + 2, y, z + 1, maxx - 2, y, z + 3); //make the actual entrance
}

void generate_house(int x, int y, int z, int size, int height, int partition_probability, int randomness, int garden, int garage, CubeType computer, CubeType green, CubeType red, CubeType purple, CubeType storage, CubeType cryofreezer, CubeType smelter, CubeType bench, CubeType crusher, CubeType regolith, CubeType steelA)
{
    printf ("Generating a house at %d, %d, %d \n", x, y, z);
    const int maxx = x + size + garden * 2 + randrange(randomness * -1, randomness);
    const int maxy = y + size + garden * 2 + randrange(randomness * -1, randomness);
    const int maxz = z + height + randrange(randomness / 4 * -1, randomness / 4);
    int partition;
    int cx = x + 1 + garden;
    int cy = y + 1 + garden;
    int colors = randrange(1, 3); //determines which part of a house is colored in one way
    generate_area(x, y, z, maxx, maxy, z, regolith); //make the garden floor
    if (colors == 1)
    {
        generate_area(x + garden, y + garden, z + 1, maxx - garden, maxy - garden, z, red); //generate the normal floor
        generate_area(x + garden, y + garden, z + 1, maxx - garden, y + garden, maxz, green); //generate a wall
        generate_area(x + garden, y + garden, z + 1, x + garden, maxy - garden, maxz, green);
        generate_area(maxx - garden, y + garden, z + 1, maxx - garden, maxy - garden, maxz, green);
        generate_area(x + garden, maxy - garden, z + 1, maxz - garden, maxy - garden, maxz, green);
        generate_area(x + 1 + garden, y + 1 + garden, maxz, maxx - 1 - garden, maxy - 1 - garden, maxz, purple); //make the roof
        while (cx < maxx - garden)
        {
            partition = randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(cx, y + 1 + garden, z + 1, cx, maxy - 1 - garden, maxz - 1, green);
            degenerate_area(cx, y + 1 + garden, z + 1, cx, y + garden + (maxy - y) / 2, maxz - 1);
            }
        cx++;
        }
        while (cy < maxy - garden)
        {
            partition = randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(x + 1 + garden, cy, z + 1, maxx - 1 - garden, cy, maxz - 1, green);
            generate_area(x + 4 + garden, cy, z + 1, maxx - 4 - garden, cy, maxz - 1, storage);
            degenerate_area(x + 1 + garden, cy, z + 1, x + garden + (maxx - x) / 2, cy, maxz - 1);
            }
        cy++;
        }
    }
    if (colors == 2)
    {
        generate_area(x + garden, y + garden, z + 1, maxx - garden, maxy - garden, z, purple); //generate the normal floor
        generate_area(x + garden, y + garden, z + 1, maxx - garden, y + garden, maxz, red); //generate a wall
        generate_area(x + garden, y + garden, z + 1, x + garden, maxy - garden, maxz, red);
        generate_area(maxx - garden, y + garden, z + 1, maxx - garden, maxy - garden, maxz, red);
        generate_area(x + garden, maxy - garden, z + 1, maxz - garden, maxy - garden, maxz, red);
        generate_area(x + 1 + garden, y + 1 + garden, maxz, maxx - 1 - garden, maxy - 1 - garden, maxz, green); //make the roof
        while (cx < maxx - garden)
        {
            partition = randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(cx, y + 1 + garden, z + 1, cx, maxy - 1 - garden, maxz - 1, red);
            degenerate_area(cx, y + 1 + garden, z + 1, cx, y + garden + (maxy - y) / 2, maxz - 1);
            }
        cx++;
        }
        while (cy < maxy - garden)
        {
            partition = randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(x + 1 + garden, cy, z + 1, maxx - 1 - garden, cy, maxz - 1, red);
            generate_area(x + 4 + garden, cy, z + 1, maxx - 4 - garden, cy, maxz - 1, storage);
            degenerate_area(x + 1 + garden, cy, z + 1, x + garden + (maxx - x) / 2, cy, maxz - 1);
            }
        cy++;
        }
    }
    if (colors == 3)
    {
        generate_area(x + garden, y + garden, z + 1, maxx - garden, maxy - garden, z, green); //generate the normal floor
        generate_area(x + garden, y + garden, z + 1, maxx - garden, y + garden, maxz, purple); //generate a wall
        generate_area(x + garden, y + garden, z + 1, x + garden, maxy - garden, maxz, purple);
        generate_area(maxx - garden, y + garden, z + 1, maxx - garden, maxy - garden, maxz, purple);
        generate_area(x + garden, maxy - garden, z + 1, maxz - garden, maxy - garden, maxz, purple);
        generate_area(x + 1 + garden, y + 1 + garden, maxz, maxx - 1 - garden, maxy - 1 - garden, maxz, red); //make the roof
        while (cx < maxx - garden)
        {
            partition = randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(cx, y + 1 + garden, z + 1, cx, maxy - 1 - garden, maxz - 1, purple);
            degenerate_area(cx, y + 1 + garden, z + 1, cx, y + garden + (maxy - y) / 2, maxz - 1);
            }
        cx++;
        }
        while (cy < maxy - garden)
        {
            partition = randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(x + 1 + garden, cy, z + 1, maxx - 1 - garden, cy, maxz - 1, purple);
            generate_area(x + 4 + garden, cy, z + 1, maxx - 4 - garden, cy, maxz - 1, storage);
            degenerate_area(x + 1 + garden, cy, z + 1, x + garden + (maxx - x) / 2, cy, maxz - 1);
            }
        cy++;
        }
    }
    generate_area(x + garden + 1, y + garden + 1, z + 1, x + garden + 1, y + garden + 1, z + 1, cryofreezer);
    generate_area(x + garden + 1, y + garden + 1, z + 2, x + garden + 1, y + garden + 1, z + 2, cryofreezer);
    generate_area(maxx - garden - 1, y + garden + 1, z + 1, maxx - garden - 1, y + garden + 1, z + 1, smelter);
    generate_area(maxx - garden - 1, maxy - garden - 1, z + 1, maxx - garden - 1, maxy - garden - 1, z + 1, bench);
    generate_area(maxx - garden - 1, maxy - garden - 2, z + 1, maxx - garden - 1, maxy - garden - 2, z + 1, crusher);
    t_map::set(x + (maxx - x) / 2, y + (maxy - y) / 2, z + 1, computer);
    generate_area(maxx - garage, maxy - garage, z, maxx, maxy, z, steelA); //start garage generation by generating the floor
    generate_area(maxx - garage + 2, maxy - garage + 2, z + 1, maxx, maxy - garage + 2, z + garage, steelA); //make one garage wall with an opening pointing towards the house
    generate_area(maxx - garage + 2, maxy - garage + 2, z + 1, maxx - garage - 2, maxy, z + garage, steelA);
    generate_area(maxx, maxy - garage, z + 1, maxx, maxy, z + garage, steelA);
    generate_area(maxx - garage, maxy - garage, z + garage, maxx, maxy, z + garage, steelA); //generate the roof for the garage
}

void generate_shop(int x, int y, int z, int size, int height, int goods, CubeType steelA, CubeType steelB, CubeType steelC, CubeType computer, CubeType storage, CubeType cryofreezer)
{
    printf ("Generating a shop at %d, %d, %d \n", x, y, z);
    generate_area(x, y, z, x + size, y + size, z, steelA);
    generate_area(x, y, z + 1, x + size, y, z + height - 1, steelB);
    generate_area(x, y, z + 1, x, y + size, z + height - 1, steelB);
    generate_area(x + size, y, z + 1, x + size, y + size, z + height - 1, steelB);
    generate_area(x, y + size, z + 1, x + size, y + size, z + height - 1, steelB);
    generate_area(x, y, z + height, x + size, y + size, z + height, steelC);
    degenerate_area(x + size / 3, y, z + 1, x + size / 2, y, z + height - 2);
    t_map::set(x + size / 2, y + size / 2, z + 1, computer);
    generate_area(x + size / 3, y + size / 3, z + 1, x + 2 * size / 3, y + size / 3, z + height - 2, storage);
    generate_area(x + size / 3, y + size * 2 / 3, z + 1, x + size * 2 / 3, y + size * 2 / 3, z + height - 2, cryofreezer);
}

void generate_transmission_tower(int x, int y, int z, int height, CubeType steelA, CubeType steelB, CubeType steelC, CubeType gray, CubeType battery, CubeType computer)
{
    printf ("Generating a transmission tower at %d, %d, %d \n", x, y, z);
    generate_area(x + 1, y + 1,z + 1, x + 1, y + 1,z + 1, battery);
    generate_area(x, y, z + 1, x, y, z + height - 1, steelC);
    generate_area(x + 1, y, z + 1, x + 1, y, z + height / 2, steelA);
    generate_area(x, y + 1, z + 1, x, y + 1, z + height / 4, steelB);
    t_map::set(x, y, z + height / 2, computer);
    t_map::set(x, y, z + height, gray);
}

void create_roads(int size, CubeType steel, int x, int y, int ox, int oy, CubeType rock, CubeType regolith)
{
    int prevheight=60;
    printf ("Generating roads from %d, %d to %d, %d\n", x, y, ox, oy);
    //set miny and minx, maxy and maxx, generate like subway tunnels, only on surface, at prevheight if gorge or treeshroom
    int minx;
    int miny;
    int maxx;
    int maxy;
    if (x < ox)
    {
        minx = x;
        maxx = ox;
    }
    else
    {
        minx = ox;
        maxx = x;
    }
    if (y  < oy)
    {
        miny = y;
        maxy = oy;
    }
    else
    {
        miny = oy;
        maxy = y;
    }
    if (t_map::get(minx, miny, t_map::get_highest_open_block(minx, miny) - 1) == rock || t_map::get(minx, miny, t_map::get_highest_open_block(minx, miny) - 1) == regolith) prevheight = t_map::get_highest_open_block(minx, miny);

    //generate one part of the road, from the middle to one end

    for (int i = minx; i <= minx + size; i++)
    for (int j = miny; j <= miny + size; j++)
    {
        if (t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) == rock || t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) == regolith)
        {
            if (t_map::get_highest_open_block(i, j) > prevheight - 2 && t_map::get_highest_open_block(i, j) < prevheight + 2)
            {
                t_map::set(i, j, t_map::get_highest_open_block(i, j), steel);
                prevheight = t_map::get_highest_open_block(i, j);
            }
            if (t_map::get_highest_open_block(i, j) <= prevheight - 2 && t_map::get_highest_open_block(i, j) >= prevheight + 2)
            {
                t_map::set(i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16, steel);
                prevheight = (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16;
            }
        }
        else if (t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) != steel)
        {
            t_map::set(i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16, steel);
            degenerate_area(i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16 + 1, i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16 + size);
        }
    }

    //now generate the second part of the road, between the other end and the middle

    for (int j = miny + size + 1; j <= maxy; j++)
    for (int i = minx + size + 1; i <= maxx; i++)
    {
        if (t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) == rock || t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) == regolith)
        {
            if (t_map::get_highest_open_block(i, j) > prevheight - 2 && t_map::get_highest_open_block(i, j) < prevheight + 2)
            {
                t_map::set(i, j, t_map::get_highest_open_block(i, j), steel);
                prevheight = t_map::get_highest_open_block(i, j);
            }
            if (t_map::get_highest_open_block(i, j) <= prevheight - 2 && t_map::get_highest_open_block(i, j) >= prevheight + 2)
            {
                t_map::set(i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16, steel);
                prevheight = (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16;
            }
        }
        else if (t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) != steel)
        {
            t_map::set(i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16, steel);
            degenerate_area(i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16 + 1, i, j, (prevheight * 15 + t_map::get_highest_open_block(i, j)) / 16 + size);
        }
    }
}

void generate_temple(int x, int y, int z, int size, CubeType glowgreen, CubeType glowblue, CubeType rock)
{
    printf ("Generating a temple at %d, %d, %d \n", x, y, z);
    generate_area(x, y, z, x + size, y + size, z, glowblue); //make the floor
    int count = 1;
    while (count <= size / 3.2) //make the walls
    {
        generate_area(x + count, y + count, z + count, x + size - count, y + count, z + count, glowgreen);
        generate_area(x + count, y + count, z + count, x + count, y + size - count, z + count, glowgreen);
        generate_area(x + count, y + size - count, z + count, x + size - count, y + size - count, z + count, glowgreen);
        generate_area(x + count, y + size - count, z + count, x + size - count, y + size - count, z + count, glowgreen);
        count++;
    }
    count = 8;
    while (count <= size / 3.2 + 6) //make the staircase hover in the air above the altar
    {
        generate_area(x + count, y + count, z + count - 5, x + size - count, y + count, z + count - 5, glowblue);
        generate_area(x + count, y + count, z + count - 5, x + count, y + size - count, z + count - 5, glowblue);
        generate_area(x + count, y + size - count, z + count - 5, x + size - count, y + size - count, z + count - 5, glowblue);
        generate_area(x + count, y + size - count, z + count - 5, x + size - count, y + size - count, z + count - 5, glowblue);
        count++;
    }
    generate_area(x + size / 2, y + size / 2, z + 1, x + size / 2 + 1, y + size / 2 + 1, z + 2, rock); //generate the altar
    degenerate_area(x + size / 2, y + size / 2, 1, x + size / 2 + 1, y + size / 2 + 1, z - 1); //generate a "well" under the altar, inside the column
}

void generate_bunker(int x, int y, int maxz, int size, int depth, int floors, int partition_probability, int randomness, CubeType gray, CubeType computer, CubeType storage, CubeType cryofreezer)
{
    printf ("Generating a bunker at %d, %d, %d \n", x, y, maxz);
    int z = maxz - depth - randrange(randomness * -1, randomness);
    const int maxx = x + size + randrange(randomness * -1, randomness);
    const int maxy = y + size + randrange(randomness * -1, randomness);
    int cx = x + 1;
    int cy = y + 1;
    int door;
    int floorcount = 1;
    generate_area(x, y, z, maxx, maxy, z, gray); //generate the lowest floor
    generate_area(x, y, z + 1, maxx, y, maxz, gray); //generate a wall
    generate_area(x, y, z + 1, x, maxy, maxz, gray); //and another wall
    generate_area(maxx, y, z + 1, maxx, maxy, maxz, gray); //and so on...
    generate_area(x, maxy, z + 1, maxx, maxy, maxz, gray);
    generate_area(x + 1, y + 1, maxz, maxx - 1, maxy - 1, maxz, gray); //generate the "roof"
    degenerate_area(x + 1, y + 1, maxz, x + 3, y + 3, maxz); //make an entrance
    while (cx < maxx)
    {
        door = randrange(1, 100);
        if (door <= partition_probability)
        {
            generate_area(cx, y + 1, z + 1, cx, maxy - 1, maxz - 1, gray);
            degenerate_area(cx, y + 1, z + 1, cx, y + (maxy - y) / 2, maxz - 1);
        }
        cx++;
    }
    while (cy < maxy)
    {
        door = randrange(1, 100);
        if (door <= partition_probability)
        {
            generate_area(x + 1, cy, z + 1, x, cy, maxz - 1, gray);
            degenerate_area(x + 1, cy, z + 1, x + (maxx - x) / 2, cy, maxz - 1);
        }
        cy++;
    }
    cx = x + 1;
    cy = y + 1;
    while (floorcount <= floors - 1)
    {
    generate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * floorcount, maxx - 1, maxy - 1, z + (maxz - z) / (floors - 1) * floorcount, gray); //generate the floors above the... ummmm... floor, you know what i mean, i hope
    degenerate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * floorcount, x + randomness, y + randomness, z + (maxz - z) / (floors - 1) * floorcount);
    floorcount++;
    }
    generate_area(x + randrange(1, 2), y + 1, z + 1, maxx - randrange(1, 2), y + 1, z + randrange(1, 3), computer); //generate the stuff inside
    generate_area(x + 1, y + randrange(1, 2), maxz - randrange (1, 3), x + 1, maxy - randrange(1, 2), maxz - 1, cryofreezer);
    generate_area(x + randrange(1, 2), maxy - 1, maxz - randrange (1, 3), maxx - randrange(1, 3), maxy - 1, maxz - 1, storage);
}

void generate_column(int x, int y, int z, int size, CubeType rock)
{
    printf ("Generating a column at %d, %d, %d \n", x, y, z);
    generate_area(x, y, 1, x + size, y + size, z - 1, rock);
    generate_area(x - 1, y - 1, z, x + size + 1, y + size + 1, z, rock);
}

void generate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz, CubeType material)
{
    if (maxz >= t_map::map_dim.z) return;
    if (maxx >= t_map::map_dim.x) return;
    if (maxy >= t_map::map_dim.y) return;
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
    if (maxz >= t_map::map_dim.z) return;
    if (maxx >= t_map::map_dim.x) return;
    if (maxy >= t_map::map_dim.y) return;
    for (int i = minx; i <= maxx; i++)
    for (int j = miny; j <= maxy; j++)
    for (int k = minz; k <= maxz; k++)
    t_map::set(i,j,k,EMPTY_CUBE);
}

void generate_tunnel(int x, int y, int z, int otherx, int othery, int size, CubeType steelA, CubeType steelB, CubeType battery, CubeType rock)
{
    printf ("Generating a subway tunnel at %d, %d, %d, other end at %d, %d, %d \n", x, y, z, otherx, othery, z);
    int minx;
    int maxx;
    int miny;
    int maxy;
    if (x > otherx)
    {
        maxx = x + size /2;
        minx = otherx - size / 2;
    }
    else
    {
        maxx = otherx + size /2;
        minx = x - size / 2;
    }
    if (y > othery)
    {
        maxy = y + size / 2;
        miny = othery - size / 2;
    }
    else
    {
        maxy = othery + size / 2;
        miny = y - size / 2;
    }
    generate_area(minx, miny, z - size / 2, minx + size, maxy, z + size / 2, rock); //generate the floor
    generate_area(minx, miny, z - size / 2, maxx, miny + size, z + size / 2, rock);
    generate_area(x, y, z - size / 2, x, maxy - size / 2, z - size / 2, battery); //generate the power rail
    generate_area(x, y, z - size / 2, maxx - size / 2, y, z - size / 2, battery);
    generate_area(minx, miny, z - size / 2, minx, maxy, z + size / 2, steelA); //generate the walls
    generate_area(minx + size, miny, z - size / 2, minx + size, maxy, z + size / 2, steelA);
    generate_area(minx, miny, z - size / 2, maxx, miny, z + size / 2, steelA);
    generate_area(minx, miny + size, z - size / 2, maxx, miny + size, z + size / 2, steelA);
    generate_area(minx, miny, z + size / 2, minx + size, miny + size, z + size / 2, steelB); //genertae the ceiling
    generate_area(maxx - size, maxy - size, z + size / 2, maxx, maxy, z + size / 2, steelB);
    degenerate_area(minx + 1, miny + 1, z - size / 2, minx + size - 1, miny + size - 1, z + size / 2); //make the tunnels empty inside
    degenerate_area(maxx - size + 1, maxy - size + 1, z - size / 2, maxx - 1, maxy - 1, z + size / 2);
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

bool isGood(int x, int y, int maxx, int maxy, CubeType rock, CubeType regolith)
{
    printf("Testing if position %d, %d to %d, %d is good for a building... \n", x, y, maxx, maxy);
    GS_ASSERT(x < maxx);
    GS_ASSERT(y < maxy);
    if (maxx >= t_map::map_dim.x || maxy >= t_map::map_dim.y || x < 0 || y < 0) return 0;
    int maxlevel=0;
    int minlevel=t_map::map_dim.z - 1;
    for (int i = x; i <= maxx; i++)
    for (int j = y; j <= maxy; j++)
    {
        if (t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) != rock && t_map::get(i, j, t_map::get_highest_open_block(i, j) - 1) != regolith)
        {
            printf("Block is neither rock nor regolith! \n");
            return 0;
        }
        if (t_map::get_highest_open_block(i, j) > maxlevel) maxlevel = t_map::get_highest_open_block(i, j);
        if (t_map::get_highest_open_block(i, j) < minlevel) minlevel = t_map::get_highest_open_block(i, j);
    }
    printf("minlevel = %d \nmaxlevel = %d \n", minlevel, maxlevel);
    if (minlevel + 40 < maxlevel) return 0;
    else return 1;
}

int get_highest_area_block(int x, int y, int maxx, int maxy)
{
    int maxh=0;
    GS_ASSERT(x >= 0 && x < t_map::map_dim.x);
    GS_ASSERT(y >= 0 && y < t_map::map_dim.y);
    GS_ASSERT(maxx >= 0 && maxx < t_map::map_dim.x);
    GS_ASSERT(maxy >= 0 && maxy < t_map::map_dim.y);
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


}   // t_gen
