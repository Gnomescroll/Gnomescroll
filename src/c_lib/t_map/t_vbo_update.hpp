#pragma once

namespace t_map
{

void t_vbo_update_init();
void t_vbo_update_end();
int update_column_VBO(struct vm_column* column);
int update_chunks() GNOMESCROLL_API;

}   // t_map
