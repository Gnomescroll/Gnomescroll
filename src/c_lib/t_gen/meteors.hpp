#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
void meteor_fall(void)
{
    const int METEOR_SIZE=5; //one side of the meteor, which is a cube - will make it have a random shape later
    bool stopgen=0;
    const int displacement=100; //the amount of additional blocks of the ore to place around the crater
    int displacedcount=0; //counts the displaced blocks
    int x=randrange (65, t_map::map_dim.x - METEOR_SIZE - 65);
    int y=randrange (65, t_map::map_dim.y - METEOR_SIZE - 65);
    int z=randrange (15, 65);
    int xcurrent=x;
    int ycurrent=y;
    int zcurrent=z;
    CubeID tile_id = NULL_CUBE;
    static CubeID rock = t_map::get_cube_id("rock");
    int meteorandomizer=randrange (1, 6); //1 makes the meteor out of iron, 2-coal(I know it's strange, delete that if you want), 3-copper, 4-iridium, 5-gallium and 6-methice
    if (meteorandomizer==1)
    while (t_map::get(x, y, z)==EMPTY_CUBE)
    {
        z=z-1;
    }
    if (meteorandomizer==1)
    {
        tile_id = t_map::get_cube_id("iron_ore");
        printf("Incoming iron meteor at %d, %d, %d!\n", x, y, z);
    }
    if (meteorandomizer==2)
    {
        tile_id = t_map::get_cube_id("coal");
        printf("Incoming coal meteor at %d, %d, %d!\n", x, y, z);
    }
    if (meteorandomizer==3)
    {
        tile_id = t_map::get_cube_id("copper_ore");
        printf("Incoming copper meteor at %d, %d, %d!\n", x, y, z);
    }
    if (meteorandomizer==4)
    {
        tile_id = t_map::get_cube_id("iridium_ore");
        printf("Incoming iridium meteor at %d, %d, %d!\n", x, y, z);
    }
    if (meteorandomizer==5)
    {
        tile_id = t_map::get_cube_id("gallium_ore");
        printf("Incoming gallium meteor at %d, %d, %d!\n", x, y, z);
    }
    if (meteorandomizer==6)
    {
        tile_id = t_map::get_cube_id("methane_ice");
        printf("Incoming methane meteor at %d, %d, %d!\n", x, y, z);
    }
    GS_ASSERT(t_map::isValidCube(tile_id));
    if (!t_map::isValidCube(tile_id)) return;
    while (!stopgen) //while the generation has not been stopped by attempting to overwrite itself
    {
        t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        if (xcurrent <= x + METEOR_SIZE)
            {
                xcurrent++;
            }
        if (xcurrent == x + METEOR_SIZE + 1)
            {
                xcurrent=x;
                if (ycurrent <= y + METEOR_SIZE)
                    {
                        ycurrent++;
                    }
                if (ycurrent == y + METEOR_SIZE + 1)
                    {
                        ycurrent=y;
                        if (zcurrent <= z + METEOR_SIZE)
                            {
                                zcurrent++;
                            }
                        if (zcurrent == z + METEOR_SIZE + 1)
                            {
                                stopgen=1;
                            }
                    }
            }
    }
    stopgen=0;
    while (zcurrent < t_map::map_dim.z)
    {
            t_map::set(xcurrent, ycurrent, zcurrent, EMPTY_CUBE);
            if (xcurrent <= x + METEOR_SIZE)
            {
                xcurrent++;
            }
        if (xcurrent == x + METEOR_SIZE + 1)
            {
                xcurrent=x;
                if (ycurrent <= y + METEOR_SIZE)
                    {
                        ycurrent++;
                    }
                if (ycurrent == y + METEOR_SIZE + 1)
                    {
                        ycurrent=y;
                        zcurrent++;
                    }
            }

    }
    while (displacedcount <= displacement)
    {
        zcurrent=z + METEOR_SIZE + randrange(1, 15);
        xcurrent=x + METEOR_SIZE + randrange(1, (100 - zcurrent) / 5);
        ycurrent=y + METEOR_SIZE + randrange(1, (100 - zcurrent) / 5);
        xcurrent=x - randrange(1, (100 - zcurrent) / 5);
        ycurrent=y - randrange(1, (100 - zcurrent) / 5);
        t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        xcurrent=xcurrent + randrange(0, 3);
        ycurrent=ycurrent + randrange(0, 3);
        xcurrent=xcurrent - randrange(0, 3);
        ycurrent=ycurrent - randrange(0, 3);
        zcurrent=zcurrent - randrange(0, 2);
        t_map::set(xcurrent, ycurrent, zcurrent, rock);
        xcurrent=xcurrent - randrange(0, 5);
        ycurrent=ycurrent - randrange(0, 5);
        xcurrent=xcurrent + randrange(0, 5);
        ycurrent=ycurrent + randrange(0, 5);
        t_map::set(xcurrent, ycurrent, zcurrent, EMPTY_CUBE);
        displacedcount++;
    }
    displacedcount=0;
    while (displacedcount <= displacement / 2)
    {
        zcurrent=z - randrange(0, 2);
        xcurrent=x - randrange(0, 2);
        ycurrent=y - randrange(0, 2);
        zcurrent=zcurrent + randrange(0, METEOR_SIZE + 2);
        xcurrent=xcurrent + randrange(0, METEOR_SIZE + 2);
        ycurrent=ycurrent + randrange(0, METEOR_SIZE + 2);
        t_map::set(xcurrent, ycurrent, zcurrent, tile_id);
        displacedcount++;
    }
    zcurrent=z + METEOR_SIZE + 1;
    xcurrent=x;
    ycurrent=y;
    displacedcount=0;
    while (zcurrent < t_map::map_dim.z)
    {
        zcurrent=zcurrent + 1;
        while(displacedcount <= displacement)
        {
            xcurrent=x - randrange(0, zcurrent / 10);
            ycurrent=y - randrange(0, zcurrent / 10);
            xcurrent=xcurrent + randrange(0, METEOR_SIZE + zcurrent /10);
            ycurrent=ycurrent + randrange(0, METEOR_SIZE + zcurrent /10);
            t_map::set(xcurrent, ycurrent, zcurrent, EMPTY_CUBE);
            displacedcount++;
        }
        displacedcount=0;
    }
}
