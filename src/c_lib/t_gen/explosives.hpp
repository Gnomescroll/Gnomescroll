#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <sound/sound.hpp>
void create_explosion(int x, int y, int z, int strength)
{
    if(x>t_map::map_dim.x) return; //stop if the location of the explosive is not valid
    if(y>t_map::map_dim.y) return;
    if(z>t_map::map_dim.z) return;
    if(x<1) return;
    if(y<1) return;
    if(z<1) return;
    if(strength * 2 + 1>t_map::map_dim.x) return; //stop if the explosion would destroy the whole map and attempt to go other the map border twice
    if(strength * 2 + 1>t_map::map_dim.y) return;
    if(strength * 2 + 1>t_map::map_dim.z) return;
    int blockedup=z; //higher z, the coords of higher degeneration block
    int blockeddown=z; //lower z
    int blockedleft=x; //lower x
    int blockedright=x; //higher x
    int blockedforward=y; //higher y
    int blockedbackward=y; //lower y
    static CubeID bedrock=t_map::get_cube_id("bedrock");
    static CubeID iron=t_map::get_cube_id("steel_block_1");
    static CubeID plasmagen=t_map::get_cube_id("plasmagen");
    int tx=x-strength;
    if(tx<1)
    {
        tx=t_map::get_map_dim.x+tx;
    }
    int ty=y-strength;
    if(ty<1)
    {
        ty=t_map::get_map_dim.y+ty;
    }
    int tz=z-strength;
    if(tz<1)
    {
        tz=t_map::get_map_dim.z+tz;
    }
    int cx=tx;
    int cy=ty;
    int cz=tz;
    bool stop=0;
    while (!stop)
    {
        if (cx>t_map::map_dim.x) cx=1; //wrap around map border
        if (cy>t_map::map_dim.y) cy=1;
        if (cz>t_map::map_dim.z) cz=t_map::map_dim.z; //clamp
        if (t_map::get(cx, cy, cz)==bedrock || t_map::get(cx, cy, cz)==iron)
        {
            if (cx<x) blockedleft=cx;
            if (cx>x) blockedright=cx;
            if (cy<y) blockedbackward=cy;
            if (cy>y) blockedforward=cy;
            if (cz<z) blockeddown=cz;
            if (cz>z) blockedup=cz;
        }
        if (cx <= tx + strength)
            {
                cx++;
            }
        if (cx >= tx + strength + 1)
            {
                cx=x;
                if (cy <= ty + strength)
                    {
                        cy++;
                    }
                if (cy >= ty + strength + 1)
                    {
                        cy=y;
                        if (cz <= tz + strength)
                            {
                                cz++;
                            }
                        if (cz >= tz + strength + 1)
                            {
                                stop=1;
                            }
                    }
            }
    }
    stop=0;
    cx=tx;
    cy=ty;
    cz=tz;
    while (!stop)
    {
        if (cx>t_map::map_dim.x) cx=1; //wrap around map border
        if (cy>t_map::map_dim.y) cy=1;
        if (cz>t_map::map_dim.z) cz=t_map::map_dim.z; //clamp
        if (!t_map::get(cx, cy, cz)==bedrock && !t_map::get(cx, cy, cz)==iron && !t_map::get(cx, cy, cz)==plasmagen && cx>blockedleft || cx<blockedright && cy>blockedbackward || cy<blockedforward && cz>blockeddown || cz<blockedup) t_map::set(cx, cy, cz, EMPTY_CUBE);
        if (t_map::get(cx, cy, cz)==plasmagen && !cx==x || !cy==y || !cz==z) create_explosion(cx, cy, cz, strength);
        if (cx <= tx + strength)
            {
                cx++;
            }
        if (cx >= tx + strength + 1)
            {
                cx=x;
                if (cy <= ty + strength)
                    {
                        cy++;
                    }
                if (cy >= ty + strength + 1)
                    {
                        cy=y;
                        if (cz <= tz + strength)
                            {
                                cz++;
                            }
                        if (cz >= tz + strength + 1)
                            {
                                stop=1;
                            }
                    }
            }
    }
}
