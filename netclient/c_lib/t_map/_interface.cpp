#include "_interface.hpp"

#if DC_CLIENT
#include "t_vbo_class.hpp"
#include "t_vbo_update.hpp"
#include "t_vbo_draw.hpp"
#endif

namespace t_map
{
    
#if DC_CLIENT
    class Vbo_map* vbo_map;

    void init_t_vbo()
    {
        vbo_map = new Vbo_map(main_map);

        t_vbo_update_init();

        vbo_draw_init();

    }

    void end_t_vbo()
    {
        delete vbo_map;
        t_vbo_update_end();
        vbo_draw_end();
    }

    void draw_map()
    {
        vbo_map->draw_map();
    }

    void update_map()
    {
        vbo_map->update_map();
    }
#endif
}

