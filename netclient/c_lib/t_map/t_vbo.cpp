#include "t_vbo.hpp"

#include "t_vbo_class.hpp"
#include "t_vbo_update.hpp"

namespace t_map
{
    
    class Vbo_map* vbo_map;

    void init_t_vbo()
    {
        vbo_map = new Vbo_map(main_map);

        t_vbo_update_init();
    }

    void end_t_vbo()
    {
        delete vbo_map;

        t_vbo_update_end();
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