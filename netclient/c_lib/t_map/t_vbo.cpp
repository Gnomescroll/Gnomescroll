#include "t_vbo.hpp"

#include "t_vbo_class.hpp"

namespace t_map
{
    
    class Vbo_map* vbo_map;

    void init_t_vbo()
    {
        vbo_map = new Vbo_map(main_map);

        vlist_scratch_0 = (struct Vertex*) malloc(16*16*(TERRAIN_MAP_HEIGHT/2)*4*sizeof(struct Vertex));
        vlist_scratch_1 = (struct Vertex*) malloc(16*16*(TERRAIN_MAP_HEIGHT/2)*4*sizeof(struct Vertex));
    }

    void end_t_vbo()
    {
        delete vbo_map;

        free(vlist_scratch_0);
        free(vlist_scratch_1);
    }

    void draw_map()
    {
        vbo_map->draw_map();
        

    }

    void update_map()
    {
        vbo_map->update_map();
    }
}