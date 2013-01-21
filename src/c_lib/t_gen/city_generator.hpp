#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{

void generate_city();
void generate_lab(int x, int y, int z, int size, int height, int floors, int randomness, int door_probability, CubeType computer, CubeType steelA, CubeType steelB, CubeType steelC, CubeType battery, CubeType smelter, CubeType cryofreezer, CubeType bench, CubeType crusher, CubeType storage);
void generate_skyscraper(int x, int y, int z, int size, int height, int floors, int randomness, int partitions, CubeType computer, CubeType purple, CubeType green, CubeType red, CubeType cryofreezer, CubeType battery);
void generate_subway_station(int x, int y, int z, int prevsubwayx, int prevsubwayy, int firstsubwayx, int firstsubwayy, bool laststation, int size, int height, int tunnel_size, CubeType gray, CubeType steelA, CubeType steelB, CubeType steelC, CubeType battery, CubeType rock);
void generate_house(int x, int y, int z, int size, int height, int partition_probability, int randomness, int garden, int garage, CubeType computer, CubeType green, CubeType red, CubeType purple, CubeType storage, CubeType cryofreezer, CubeType smelter, CubeType bench, CubeType crusher, CubeType regolith, CubeType steelA);
void generate_shop(int x, int y, int z, int size, int height, int goods, CubeType steelA, CubeType steelB, CubeType steelC, CubeType computer, CubeType storage, CubeType cryofreezer);
void generate_transmission_tower(int x, int y, int z, int height, CubeType steelA, CubeType steelB, CubeType steelC, CubeType gray, CubeType battery, CubeType computer);
void degenerate_space(int x, int y, int z, int size);
void create_floor(int x, int y, int z, int size, CubeType gray);
void create_roads(int size, CubeType steel, int minx, int miny, int maxx, int maxy);
void generate_temple(int x, int y, int z, int size, CubeType glowgreen, CubeType glowblue, CubeType rock);
void generate_bunker(int x, int y, int maxz, int size, int depth, int floors, int partition_probability, int randomness, CubeType gray, CubeType computer, CubeType storage, CubeType cryofreezer);
void generate_column(int x, int y, int z, int size, CubeType rock);
void generate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz, CubeType material);
void degenerate_area(int minx, int miny, int minz, int maxx, int maxy, int maxz);
void generate_tunnel(int x, int y, int z, int otherx, int othery, int size, CubeType steelA, CubeType steelB, CubeType battery, CubeType rock);
void create_cryofreezer(int x, int y, int z);
void create_storage(int x, int y, int z);
void create_smelter(int x, int y, int z);
void create_bench(int x, int y, int z);
void create_crusher(int x, int y, int z);
bool isGood(int x, int y, int maxx, int maxy, int maxheight, CubeType rock);
bool isRoad(int x, int y, int size);

}   // t_gen
