#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{
void generate_city();
void generate_lab(const int x, const int y, const int z, const int size, const int height, const int floors, const int randomness, const int door_probability, CubeID computer, CubeID steelA, CubeID steelB, CubeID steelC, CubeID battery, CubeID smelter, CubeID cryofreezer, CubeID bench, CubeID crusher, CubeID storage);
void generate_skyscraper(const int x, const int y, const int z, const int size, const int height, const int floors, const int randomness, const int partitions, CubeID computer, CubeID purple, CubeID green, CubeID red, CubeID cryofreezer, CubeID battery);
void generate_subway_station(const int x, const int y, const int z, const int prevsubwayx, const int prevsubwayy, const int firstsubwayx, const int firstsubwayy, const bool laststation, const int size, const int height, const int tunnel_size, CubeID gray, CubeID steelA, CubeID steelB, CubeID steelC, CubeID battery, CubeID rock);
void generate_house(const int x, const int y, const int z, const int size, const int height, const int partition_probability, const int randomness, const int garden, const int garage, CubeID computer, CubeID green, CubeID red, CubeID purple, CubeID storage, CubeID cryofreezer, CubeID smelter, CubeID bench, CubeID crusher, CubeID regolith, CubeID steelA);
void generate_shop(const int x, const int y, const int z, const int size, const int height, const int goods, CubeID steelA, CubeID steelB, CubeID steelC, CubeID computer, CubeID storage, CubeID cryofreezer);
void generate_transmission_tower(const int x, const int y, const int z, const int height, CubeID steelA, CubeID steelB, CubeID steelC, CubeID gray, CubeID battery, CubeID computer);
void degenerate_space(const int x, const int y, const int z, const int size);
void create_floor(const int x, const int y, const int z, const int size, CubeID gray);
void create_roads(const int x, const int y, const int z, const int city_size, const int size, CubeID rock);
void generate_temple(const int x, const int y, const int z, const int size, CubeID glowgreen, CubeID glowblue, CubeID rock);
void generate_bunker(const int x, const int y, const int maxz, const int size, const int depth, const int floors, const int partition_probability, const int randomness, CubeID gray, CubeID computer, CubeID storage, CubeID cryofreezer);
void generate_column(const int x, const int y, const int z, const int size, CubeID rock);
void generate_area(const int minx, const int miny, const int minz, const int maxx, const int maxy, const int maxz, CubeID material);
void degenerate_area(const int minx, const int miny, const int minz, const int maxx, const int maxy, const int maxz);
void generate_tunnel(const int x, const int y, const int z, const int otherx, const int othery, const int size, CubeID steelA, CubeID steelB, CubeID battery, CubeID rock);
void create_cryofreezer(const int x, const int y, const int z);
void create_storage(const int x, const int y, const int z);
void create_smelter(const int x, const int y, const int z);
void create_bench(const int x, const int y, const int z);
void create_crusher(const int x, const int y, const int z);
}
