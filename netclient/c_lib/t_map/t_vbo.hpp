#pragma once

namespace t_map
{

    static struct Vertex* vlist_scratch_0;
    static struct Vertex* vlist_scratch_1;

    void init_t_vbo();

    void draw_map() GNOMESCROLL_API;
    void update_map();
}
