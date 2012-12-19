#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{
    
void generate_city();
void generate_lab(int x, int y, int z, int size, int height, int floors, int randomness, int door_probability, CubeID computer, CubeID steelA, CubeID steelB, CubeID steelC, CubeID battery, CubeID smelter, CubeID cryofreezer, CubeID bench, CubeID crusher, CubeID storage);
void generate_skyscraper(int x, int y, int z, int size, int height, int floors, int randomness, int partitions, CubeID computer, CubeID purple, CubeID green, CubeID red, CubeID cryofreezer, CubeID battery);
void generate_subway_station(int x, int y, int z, int prevsubwayx, int prevsubwayy, int firstsubwayx, int firstsubwayy, bool laststation, int size, int height, int tunnel_size, CubeID gray, CubeID steelA, CubeID steelB, CubeID steelC, CubeID battery, CubeID rock);
void generate_house(int x, int y, int z, int size, int height, int partition_probability, int randomness, int garden, int garage, CubeID computer, CubeID green, CubeID red, CubeID purple, CubeID storage, CubeID cryofreezer, CubeID smelter, CubeID bench, CubeID crusher, CubeID regolith, CubeID steelA);
void generate_shop(int x, int y, int z, int size, int height, int goods, CubeID steelA, CubeID steelB, CubeID steelC, CubeID computer, CubeID storage, CubeID cryofreezer);
void generate_transmission_tower(int x, int y, int z, int height, CubeID steelA, CubeID steelB, CubeID steelC, CubeID gray, CubeID battery, CubeID computer);
void degenerate_space(int x, int y, int z, int size);
void create_floor(int x, int y, int z, int size, CubeID gray);
void create_roads(int x, int y, int z, int city_size, int size, CubeID rock);
void generate_temple(int x, int y, int z, int size, CubeID glowgreen, CubeID glowblue, CubeID rock);
void generate_bunker(int x, int y, int maxz, int size, int depth, int floors, int partition_probability, int randomness, CubeID gray, CubeID computer, CubeID storage, CubeID cryofreezer);
void generate_column(int x, int y, int z, int size, CubeID rock);
void generate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz, CubeID material);
void degenerate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz);
void generate_tunnel(int x, int y, int z, int otherx, int othery, int size, CubeID steelA, CubeID steelB, CubeID battery, CubeID rock);
void create_cryofreezer(int x, int y, int z);
void create_storage(int x, int y, int z);
void create_smelter(int x, int y, int z);
void create_bench(int x, int y, int z);
void create_crusher(int x, int y, int z);

}   // t_gen
