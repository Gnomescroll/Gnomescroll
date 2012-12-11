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
    const int CITY_SIZE=128; //the average horizontal size of a city in blocks
    const int CITY_RANDOMNESS=32; //the maximal amount of blocks by which size can change
    const int CITY_AMOUNT=1;
    const int MINIMAL_ELEVATION=30;
    const int MAXIMAL_ELEVATION=60;
    const int LAB_SIZE=16;
    const int LAB_DOOR_PROBABILITY=10; //the probability of a partition wall generating, in percent
    const int LAB_HEIGHT=12;
    const int LAB_RANDOMNESS=4;
    const int LAB_FLOORS=2;
    const int SKYSCRAPER_SIZE=16;
    const int SKYSCRAPER_FLOORS=10;
    const int SKYSCRAPER_PARTITIONS=3;
    const int SKYSCRAPER_HEIGHT=60;
    const int SKYSCRAPER_RANDOMNESS=4;
    const int SUBWAY_STATION_SIZE=8;
    const int SUBWAY_STATION_HEIGHT=6;
    const int SUBWAY_TUNNEL_SIZE=5;
    const int SUBWAY_LOOP_STATIONS=8;
    const int BUNKER_SIZE=16;
    const int BUNKER_DEPTH=8;
    const int BUNKER_FLOORS=3;
    const int BUNKER_RANDOMNESS=4;
    const int BUNKER_PARTITION_PROBABILITY=15;
    const int TEMPLE_SIZE=32;
    const int HOUSE_SIZE=16;
    const int HOUSE_RANDOMNESS=8;
    const int HOUSE_PARTITION_PROBABILITY=8;
    const int HOUSE_HEIGHT=12;
    const int HOUSE_GARDEN=6;
    const int HOUSE_GARAGE_SIZE=4;
    const int SHOP_SIZE=8;
    const int SHOP_GOODS_AMOUNT=3;
    const int SHOP_HEIGHT=5;
    const int TRANSMISSION_TOWER_HEIGHT=64;
    const int ROAD_SIZE=4;
    const int COLUMN_SIZE=8;
    const int BUILDING_AMOUNT=8;
    CubeID computer=t_map::get_cube_id("control_node");
    CubeID green=t_map::get_cube_id("ruins_ceiling1");
    CubeID red=t_map::get_cube_id("ruins_ceiling2");
    CubeID purple=t_map::get_cube_id("ruins_ceiling3");
    CubeID gray=t_map::get_cube_id("ruins_ceiling4");
    CubeID storage=t_map::get_cube_id("storage_block_small");
    CubeID cryofreezer=t_map::get_cube_id("cryofreezer_small");
    CubeID smelter=t_map::get_cube_id("smelter_basic");
    CubeID bench=t_map::get_cube_id("crafting_bench_basic");
    CubeID crusher=t_map::get_cube_id("crusher");
    CubeID steelA=t_map::get_cube_id("steel_block_1");
    CubeID steelB=t_map::get_cube_id("steel_block_2");
    CubeID steelC=t_map::get_cube_id("steel_block_3");
    CubeID glowgreen=t_map::get_cube_id("green_glow");
    CubeID glowblue=t_map::get_cube_id("blue_glow");
    CubeID battery=t_map::get_cube_id("battery2");
    CubeID rock=t_map::get_cube_id("rock");
    CubeID regolith=t_map::get_cube_id("regolith");
    int x;
    int y;
    int z;
    int cx;
    int cy;
    int count=0;
    int prevsubwayx;
    int prevsubwayy;
    int firstsubwayx;
    int firstsubwayy;
    int subwaycounter;
    bool laststation;
    int building_randomizer;
    int size;
    while (count < CITY_AMOUNT)
    {
        x=randrange(4, t_map::map_dim.x - CITY_SIZE - CITY_RANDOMNESS - 4);
        y=randrange(4, t_map::map_dim.y - CITY_SIZE - CITY_RANDOMNESS - 4);
        z=randrange(MINIMAL_ELEVATION, MAXIMAL_ELEVATION);
        printf("Generating alien city at %d, %d, %d", x, y, z);
        cx=x;
        cy=y;
        size=CITY_SIZE + randrange(CITY_RANDOMNESS * -1, CITY_RANDOMNESS);
        subwaycounter=1;
        degenerate_space(x, y, z, size);
        create_floor(x, y, z, size, gray);
        create_roads(x, y, z, size, ROAD_SIZE, rock);
        while (cy < size)
        {
            if (cx + CITY_RANDOMNESS >= size)
            {
                cy=cy + randrange(CITY_RANDOMNESS, CITY_RANDOMNESS * 1.5) + 1;
                cx=x + randrange (0, CITY_RANDOMNESS);
            }
            if (cx + TEMPLE_SIZE / 2> x + (x + size) / 2 && cx - TEMPLE_SIZE / 2< x + (x + size) / 2)
            {
                if (cy + TEMPLE_SIZE / 2> y + (y + size) / 2 && cy - TEMPLE_SIZE / 2< y + (y + size) / 2)
                {
                    generate_temple(cx, cy, z, TEMPLE_SIZE, glowgreen, glowblue, rock);
                    generate_column(cx, cy, z - 1, TEMPLE_SIZE, rock);
                    cx=cx + TEMPLE_SIZE + randrange(1, 4);
                }
            }
            building_randomizer=randrange(1, BUILDING_AMOUNT); //1 is lab, 2 is skyscraper, 3 is subway station, 4 is house, 5 is shop, 6 is transmission tower, 7 is a square, 8 is bunker
            if (building_randomizer==1)
            {
                generate_lab(cx, cy, z, LAB_SIZE, LAB_HEIGHT, LAB_FLOORS, LAB_RANDOMNESS, LAB_DOOR_PROBABILITY, computer, steelA, steelB, steelC, battery, smelter, cryofreezer, bench, crusher, storage);
                cx=cx + LAB_SIZE + LAB_RANDOMNESS * randrange(1, 2) + 1;
            }
            if (building_randomizer==2)
            {
                generate_skyscraper(cx, cy, z, SKYSCRAPER_SIZE, SKYSCRAPER_HEIGHT, SKYSCRAPER_FLOORS, SKYSCRAPER_RANDOMNESS, SKYSCRAPER_PARTITIONS, computer, purple, green, red, cryofreezer, battery);
                generate_column(cx, cy, z - 1, COLUMN_SIZE, rock);
                cx=cx + SKYSCRAPER_SIZE + SKYSCRAPER_RANDOMNESS * randrange(1, 2) + 1;
            }
            if (building_randomizer==3)
            {
                if (subwaycounter==1)
                {
                    firstsubwayx=cx;
                    firstsubwayy=cy;
                }
                if (subwaycounter == SUBWAY_LOOP_STATIONS)
                {
                    laststation=1;
                    subwaycounter=1;
                }
                else laststation=0;
                generate_subway_station(cx, cy, z, prevsubwayx, prevsubwayy, firstsubwayx, firstsubwayy, laststation, SUBWAY_STATION_SIZE, SUBWAY_STATION_HEIGHT, SUBWAY_TUNNEL_SIZE, gray, steelA, steelB, steelC, battery, rock);
                subwaycounter++;
                prevsubwayx=cx;
                prevsubwayy=cy;
                cx=cx + SUBWAY_STATION_SIZE + randrange (1, 4);
            }
            if (building_randomizer==4)
            {
                generate_house(cx, cy, z, HOUSE_SIZE, HOUSE_HEIGHT, HOUSE_PARTITION_PROBABILITY, HOUSE_RANDOMNESS, HOUSE_GARDEN, HOUSE_GARAGE_SIZE, computer, green, red, purple, storage, cryofreezer, smelter, bench, crusher, regolith, steelA);
                cx=cx + HOUSE_SIZE + HOUSE_RANDOMNESS + HOUSE_GARDEN * 2 + 1;
            }
            if (building_randomizer==5)
            {
                generate_shop(cx, cy, z, SHOP_SIZE, SHOP_HEIGHT, SHOP_GOODS_AMOUNT, steelA, steelB, steelC, computer, storage, cryofreezer);
                cx=cx + SHOP_SIZE + randrange(1, SHOP_SIZE / 2);
            }
            if (building_randomizer==6)
            {
                generate_transmission_tower(cx, cy, z, TRANSMISSION_TOWER_HEIGHT, steelA, steelB, steelC, gray, battery, computer);
                cx=cx + 4 + randrange(1, 4);
            }
            if (building_randomizer==7)
            {
                cx=cx + randrange(CITY_RANDOMNESS / 2, CITY_RANDOMNESS);
            }
            if (building_randomizer==8)
            {
                generate_bunker(cx, cy, z, BUNKER_SIZE, BUNKER_DEPTH, BUNKER_FLOORS, BUNKER_PARTITION_PROBABILITY, BUNKER_RANDOMNESS, gray, computer, storage, cryofreezer);
            }
        }
        count++;
    }
}

void generate_lab(const int x, const int y, const int z, const int size, const int height, const int floors, const int randomness, const int door_probability, CubeID computer, CubeID steelA, CubeID steelB, CubeID steelC, CubeID battery, CubeID smelter, CubeID cryofreezer, CubeID bench, CubeID crusher, CubeID storage)
{
    int maxx=x + randrange(randomness * -1, randomness) + size;
    int maxy=y + randrange(randomness * -1, randomness) + size;
    int maxz=z + height + randrange(randomness * -1, randomness);
    int cx=x + 1;
    int cy=y + 1;
    int door;
    int floorcount=1;
    generate_area(x, y, z, maxx, maxy, z, steelA); //generate the floor
    generate_area(x, y, z + 1, maxx, y, maxz, steelB); //generate a wall
    generate_area(x, y, z + 1, x, maxy, maxz, steelB);
    generate_area(maxx, y, z + 1, maxx, maxy, maxz, steelB);
    generate_area(x, maxy, z + 1, maxx, maxy, maxz, steelB);
    generate_area(x + 1, y + 1, maxz, maxx - 1, maxy - 1, maxz, steelC); //generate the roof
    while (cx < maxx)
    {
        door=randrange(1, 100);
        if (door <= door_probability)
        {
            generate_area(cx, y + 1, z + 1, cx, maxy - 1, maxz - 1, steelA);
            degenerate_area(cx, y + 1, z + 1, cx, y + (maxy - y) / 2, maxz - 1);
        }
        cx++;
    }
    while (cy < maxy)
    {
        door=randrange(1, 100);
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

void generate_skyscraper(const int x, const int y, const int z, const int size, const int height, const int floors, const int randomness, const int partitions, CubeID computer, CubeID purple, CubeID green, CubeID red, CubeID cryofreezer, CubeID battery)
{
    int maxx=x + randrange(randomness * -1, randomness) + size;
    int maxy=y + randrange(randomness * -1, randomness) + size;
    int maxz=z + height + randrange(randomness * -1, randomness);
    int count=1;
    generate_area(x, y, z, maxx, maxy, z, red); //generate the floor
    generate_area(x, y, z + 1, maxx, y, maxz - 1, purple); //make walls
    generate_area(x, y, z + 1, x, maxy, maxz - 1, purple);
    generate_area(maxx, y, z + 1, maxx, maxy, maxz - 1, purple);
    generate_area(x, maxy, z + 1, maxx, maxy, maxz - 1, purple);
    generate_area(x, y, maxz, maxx, maxy, maxz, green); //generate the roof
    while (count <= partitions)
    {
        generate_area(x + (x + maxx) / 2 / partitions * count, y + 1, z + 1, x + (x + maxx) / 2 / partitions * count, maxy - 1, maxz - 1, green); //generate partitions
        degenerate_area(x + (x + maxx) / 2 / partitions * count, y + randomness, z + 1, x + (x + maxx) / 2 / partitions * count, maxy - randomness, maxz - 1); //make entrances to rooms
        count++;
    }
    count = 1;
    while (count <= partitions)
    {
        generate_area(x + 1, y + (y + maxy) / 2 / partitions * count, z + 1, maxx - 1, y + (y + maxy) / 2 / partitions * count, maxz - 1, green); //generate partitions on y axis
        degenerate_area(x + randomness, y + (y + maxy) / 2 / partitions * count, z + 1, maxx - randomness, y + (y + maxy) / 2 / partitions * count, maxz - 1); //make entrances to rooms
        count++;
    }
    count = 1;
    while (count < floors)
    {
        generate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * count, maxx - 1, maxy - 1, z + (maxz - z) / (floors - 1) * count, green); //generate the floors above ground
        degenerate_area(x + 1, y + 1, (z + (maxz - z) / (floors - 1)) * count, x + randomness, y + randomness, z + (maxz - z) / (floors - 1) * count); //degenerate entrances to upper floors
        count++;
    }
    count = 1;
    while (count < floors)
    {
        t_map::set(x + 2, y + 2, (z + (maxz - z) / (floors - 1)) * count + 1, red); //generate computer desks on each floor
        t_map::set(x + 2, y + 2, (z + (maxz - z) / (floors - 1)) * count + 2, computer); //generate computers on the desks
        count++;
    }
    count = 1;
    while (count < floors)
    {
        t_map::set(x + 8, y + 8, (z + (maxz - z) / (floors - 1)) * count + 1, battery); //generate a battery for "powering" the fridge above
        generate_area(x + 8, y + 8, (z + (maxz - z) / (floors - 1)) * count + 2, x + 8, y + 8, (z + (maxz - z) / (floors - 1)) * count + 2, cryofreezer); //generate the "fridge"-a cryofreezer
        count++;
    }
    degenerate_area(x + randomness, y, z + 1, maxx - randomness, y, z + 3); //create an exit
}

void generate_subway_station(const int x, const int y, const int z, const int prevsubwayx, const int prevsubwayy, const int firstsubwayx, const int firstsubwayy, const bool laststation, const int size, const int height, const int tunnel_size, CubeID gray, CubeID steelA, CubeID steelB, CubeID steelC, CubeID battery, CubeID rock)
{
    int maxx=x + size;
    int maxy=y + size;
    int minz=z - height;
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

void generate_house(const int x, const int y, const int z, const int size, const int height, const int partition_probability, const int randomness, const int garden, const int garage, CubeID computer, CubeID green, CubeID red, CubeID purple, CubeID storage, CubeID cryofreezer, CubeID smelter, CubeID bench, CubeID crusher, CubeID regolith, CubeID steelA)
{
    const int maxx=x + size + garden * 2 + randrange(randomness * -1, randomness);
    const int maxy=y + size + garden * 2 + randrange(randomness * -1, randomness);
    const int maxz=z + height + randrange(randomness / 4 * -1, randomness / 4);
    int partition;
    int cx=x + 1 + garden;
    int cy=y + 1 + garden;
    int colors=randrange(1, 3); //determines which part of a house is colored in one way
    generate_area(x, y, z, maxx, maxy, z, regolith); //make the garden floor
    if (colors==1)
    {
        generate_area(x + garden, y + garden, z + 1, maxx - garden, maxy - garden, z, red); //generate the normal floor
        generate_area(x + garden, y + garden, z + 1, maxx - garden, y + garden, maxz, green); //generate a wall
        generate_area(x + garden, y + garden, z + 1, x + garden, maxy - garden, maxz, green);
        generate_area(maxx - garden, y + garden, z + 1, maxx - garden, maxy - garden, maxz, green);
        generate_area(x + garden, maxy - garden, z + 1, maxz - garden, maxy - garden, maxz, green);
        generate_area(x + 1 + garden, y + 1 + garden, maxz, maxx - 1 - garden, maxy - 1 - garden, maxz, purple); //make the roof
        while(cx < maxx - garden)
        {
            partition=randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(cx, y + 1 + garden, z + 1, cx, maxy - 1 - garden, maxz - 1, green);
            degenerate_area(cx, y + 1 + garden, z + 1, cx, y + garden + (maxy - y) / 2, maxz - 1);
            }
        cx++;
        }
        while(cy < maxy - garden)
        {
            partition=randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(x + 1 + garden, cy, z + 1, maxx - 1 - garden, cy, maxz - 1, green);
            generate_area(x + 4 + garden, cy, z + 1, maxx - 4 - garden, cy, maxz - 1, storage);
            degenerate_area(x + 1 + garden, cy, z + 1, x + garden + (maxx - x) / 2, cy, maxz - 1);
            }
        cy++;
        }
    }
    if (colors==2)
    {
        generate_area(x + garden, y + garden, z + 1, maxx - garden, maxy - garden, z, purple); //generate the normal floor
        generate_area(x + garden, y + garden, z + 1, maxx - garden, y + garden, maxz, red); //generate a wall
        generate_area(x + garden, y + garden, z + 1, x + garden, maxy - garden, maxz, red);
        generate_area(maxx - garden, y + garden, z + 1, maxx - garden, maxy - garden, maxz, red);
        generate_area(x + garden, maxy - garden, z + 1, maxz - garden, maxy - garden, maxz, red);
        generate_area(x + 1 + garden, y + 1 + garden, maxz, maxx - 1 - garden, maxy - 1 - garden, maxz, green); //make the roof
        while(cx < maxx - garden)
        {
            partition=randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(cx, y + 1 + garden, z + 1, cx, maxy - 1 - garden, maxz - 1, red);
            degenerate_area(cx, y + 1 + garden, z + 1, cx, y + garden + (maxy - y) / 2, maxz - 1);
            }
        cx++;
        }
        while(cy < maxy - garden)
        {
            partition=randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(x + 1 + garden, cy, z + 1, maxx - 1 - garden, cy, maxz - 1, red);
            generate_area(x + 4 + garden, cy, z + 1, maxx - 4 - garden, cy, maxz - 1, storage);
            degenerate_area(x + 1 + garden, cy, z + 1, x + garden + (maxx - x) / 2, cy, maxz - 1);
            }
        cy++;
        }
    }
    if (colors==3)
    {
        generate_area(x + garden, y + garden, z + 1, maxx - garden, maxy - garden, z, green); //generate the normal floor
        generate_area(x + garden, y + garden, z + 1, maxx - garden, y + garden, maxz, purple); //generate a wall
        generate_area(x + garden, y + garden, z + 1, x + garden, maxy - garden, maxz, purple);
        generate_area(maxx - garden, y + garden, z + 1, maxx - garden, maxy - garden, maxz, purple);
        generate_area(x + garden, maxy - garden, z + 1, maxz - garden, maxy - garden, maxz, purple);
        generate_area(x + 1 + garden, y + 1 + garden, maxz, maxx - 1 - garden, maxy - 1 - garden, maxz, red); //make the roof
        while(cx < maxx - garden)
        {
            partition=randrange(1, 100);
            if (partition <= partition_probability)
            {
            generate_area(cx, y + 1 + garden, z + 1, cx, maxy - 1 - garden, maxz - 1, purple);
            degenerate_area(cx, y + 1 + garden, z + 1, cx, y + garden + (maxy - y) / 2, maxz - 1);
            }
        cx++;
        }
        while(cy < maxy - garden)
        {
            partition=randrange(1, 100);
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

void generate_shop(const int x, const int y, const int z, const int size, const int height, const int goods, CubeID steelA, CubeID steelB, CubeID steelC, CubeID computer, CubeID storage, CubeID cryofreezer)
{
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

void generate_transmission_tower(const int x, const int y, const int z, const int height, CubeID steelA, CubeID steelB, CubeID steelC, CubeID gray, CubeID battery, CubeID computer)
{
    generate_area(x + 1, y + 1,z + 1, x + 1, y + 1,z + 1, battery);
    generate_area(x, y, z + 1, x, y, z + height - 1, steelC);
    generate_area(x + 1, y, z + 1, x + 1, y, z + height / 2, steelA);
    generate_area(x, y + 1, z + 1, x, y + 1, z + height / 4, steelB);
    t_map::set(x, y, z + height / 2, computer);
    t_map::set(x, y, z + height, gray);
}

void degenerate_space(const int x, const int y, const int z, const int size)
{
    int cx=x;
    int cy=y;
    int cz=z;
    while (1)
    {
        t_map::set(cx, cy, cz, EMPTY_CUBE);
        if (cx <= x + size)
        {
            cx++;
        }
        if (cx > x + size)
        {
            cx = x;
            if (cy <= y + size)
            {
                cy++;
            }
            if (cy > y + size)
            {
                cy = y;
                if (cz <= t_map::map_dim.z)
                {
                    cz++;
                }
                if (cz > t_map::map_dim.z)
                {
                    break;
                }
            }
        }
    }
}

void create_floor(const int x, const int y, const int z, const int size, CubeID gray)
{
    int cx=x;
    int cy=y;
    while (1)
    {
        t_map::set(cx, cy, z, gray);
        if (cx <= x + size)
        {
            cx++;
        }
        if (cx > x + size)
        {
            cx = x;
            if (cy <= y + size)
            {
                cy++;
            }
            if (cy > y + size)
            {
                break;
            }
        }
    }
}

void create_roads(const int x, const int y, const int z, const int city_size, const int size, CubeID rock)
{
    generate_area(x + city_size / 2 - size / 2, 0, z, x + city_size / 2 + size / 2, y, z, rock); //generate trade routes going out of the city to the borders of the map, so that they connect on ends
    generate_area(0, y + city_size / 2 - size / 2, z, x, y + city_size / 2 + size / 2, z, rock);
    generate_area(x + city_size / 2 - size / 2, y + city_size, z, x + city_size / 2 + size / 2, t_map::map_dim.y - 1, z, rock);
    generate_area(x + city_size, y + city_size / 2 - size / 2, z, t_map::map_dim.x - 1, y + city_size / 2 + size / 2, z, rock);
    degenerate_area(x + city_size / 2 - size / 2, 0, z + 1, x + city_size / 2 + size / 2, y, z + size); //make sure that the road can be walked on
    degenerate_area(0, y + city_size / 2 - size / 2, z + 1, x, y + city_size / 2 + size / 2, z + size);
    degenerate_area(x + city_size / 2 - size / 2, y + city_size, z + 1, x + city_size / 2 + size / 2, t_map::map_dim.y - 1, z + size);
    degenerate_area(x + city_size, y + city_size / 2 - size / 2, z + 1, t_map::map_dim.x - 1, y + city_size / 2 + size / 2, z + size);
}

void generate_temple(const int x, const int y, const int z, const int size, CubeID glowgreen, CubeID glowblue, CubeID rock)
{
    generate_area(x, y, z, x + size, y + size, z, glowblue); //make the floor
    int count=1;
    while (count <= size / 3.2) //make the walls
    {
    generate_area(x + count, y + count, z + count, x + size - count, y + count, z + count, glowgreen);
    generate_area(x + count, y + count, z + count, x + count, y + size - count, z + count, glowgreen);
    generate_area(x + count, y + size - count, z + count, x + size - count, y + size - count, z + count, glowgreen);
    generate_area(x + count, y + size - count, z + count, x + size - count, y + size - count, z + count, glowgreen);
    count++;
    }
    count=7;
    while (count <= size / 3.2 + 5) //make the staircase hover in the air above the altar
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

void generate_bunker(const int x, const int y, const int maxz, const int size, const int depth, const int floors, const int partition_probability, const int randomness, CubeID gray, CubeID computer, CubeID storage, CubeID cryofreezer)
{
    const int z=maxz - depth - randrange(randomness * -1, randomness);
    const int maxx=x + size + randrange(randomness * -1, randomness);
    const int maxy=y + size + randrange(randomness * -1, randomness);
    int cx=x + 1;
    int cy=y + 1;
    int door;
    int floorcount=1;
    generate_area(x, y, z, maxx, maxy, z, gray); //generate the lowest floor
    generate_area(x, y, z + 1, maxx, y, maxz, gray); //generate a wall
    generate_area(x, y, z + 1, x, maxy, maxz, gray); //and another wall
    generate_area(maxx, y, z + 1, maxx, maxy, maxz, gray); //and so on...
    generate_area(x, maxy, z + 1, maxx, maxy, maxz, gray);
    generate_area(x + 1, y + 1, maxz, maxx - 1, maxy - 1, maxz, gray); //generate the "roof"
    degenerate_area(x + 1, y + 1, maxz, x + 3, y + 3, maxz); //make an entrance
    while (cx < maxx)
    {
        door=randrange(1, 100);
        if (door <= partition_probability)
        {
            generate_area(cx, y + 1, z + 1, cx, maxy - 1, maxz - 1, gray);
            degenerate_area(cx, y + 1, z + 1, cx, y + (maxy - y) / 2, maxz - 1);
        }
        cx++;
    }
    while (cy < maxy)
    {
        door=randrange(1, 100);
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

void generate_column(const int x, const int y, const int z, const int size, CubeID rock)
{
    generate_area(x, y, 1, x + size, y + size, z - 1, rock);
    generate_area(x - 1, y - 1, z, x + size + 1, y + size + 1, z, rock);
}

void generate_area(const int minx, const int miny, const int minz, const int maxx, const int maxy, const int maxz, CubeID material)
{
    if (maxz >= t_map::map_dim.z) return;
    if (maxx >= t_map::map_dim.x) return;
    if (maxy >= t_map::map_dim.y) return;
    int special=0;
    if (material==t_map::get_cube_id("cryofreezer_small"))
    {
        special=1;
    }
    if (material==t_map::get_cube_id("storage_block_small"))
    {
        special=2;
    }
    if (material==t_map::get_cube_id("smelter_basic"))
    {
        special=3;
    }
    if (material==t_map::get_cube_id("crafting_bench_basic"))
    {
        special=4;
    }
    if (material==t_map::get_cube_id("crusher"))
    {
        special=5;
    }
    if (special == 0)
    {
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    t_map::set(i,j,k,material);
    }
    if (special == 1)
    {
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    create_cryofreezer(i,j,k);
    }
    if (special == 2)
    {
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    create_storage(i,j,k);
    }
    if (special == 3)
    {
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    create_smelter(i,j,k);
    }
    if (special == 4)
    {
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    create_bench(i,j,k);
    }
    if (special == 5)
    {
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    create_crusher(i,j,k);
    }
}

void degenerate_area(const int minx, const int miny, const int minz, const int maxx, const int maxy, const int maxz)
{
    for (int i=minx; i<=maxx; i++)
    for (int j=miny; j<=maxy; j++)
    for (int k=minz; k<=maxz; k++)
    t_map::set(i,j,k,EMPTY_CUBE);
}

void generate_tunnel(const int x, const int y, const int z, const int otherx, const int othery, const int size, CubeID steelA, CubeID steelB, CubeID battery, CubeID rock)
{
    int minx;
    int maxx;
    int miny;
    int maxy;
    if (x > otherx)
    {
        maxx=x + size /2;
        minx=otherx - size / 2;
    }
    else
    {
        maxx=otherx + size /2;
        minx=x - size / 2;
    }
    if (y > othery)
    {
        maxy=y + size / 2;
        miny=othery - size / 2;
    }
    else
    {
        maxy=othery + size / 2;
        miny=y - size / 2;
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

void create_cryofreezer(const int x, const int y, const int z)
{
    int id = ItemContainer::create_container_block("cryofreezer_small", x, y, z);
    if (id != NULL_CONTAINER) { ItemContainer::auto_add_item_to_container("methane_ice", id); }
}

void create_storage(const int x, const int y, const int z)
{
    int id = ItemContainer::create_container_block("storage_block_small", x, y, z);
    if (id != NULL_CONTAINER)
    {
        ItemContainer::auto_add_item_to_container("iron_ingot", id);
        ItemContainer::auto_add_item_to_container("iridium_ingot", id);
        ItemContainer::auto_add_item_to_container("gallium_ingot", id);
        ItemContainer::auto_add_item_to_container("coal", id);
    }
}

void create_smelter(const int x, const int y, const int z)
{
    ItemContainer::create_container_block("smelter_basic", x, y, z);
}

void create_bench(const int x, const int y, const int z)
{
    ItemContainer::create_container_block("crafting_bench_basic", x, y, z);
}

void create_crusher(const int x, const int y, const int z)
{
    ItemContainer::create_container_block("crusher", x, y, z);
}
}
