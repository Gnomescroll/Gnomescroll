#include "t_vbo.hpp"

#include "t_vbo_class.hpp"

namespace t_map
{
    
    class Vbo_map* vbo_map;

    void init_t_vbo()
    {
        vbo_map = new Vbo_map(main_map);
    }

    void draw_map()
    {
        
    }
}