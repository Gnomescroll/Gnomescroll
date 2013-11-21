/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace t_map
{

void t_vbo_update_init();
void t_vbo_update_end();
int update_column_VBO(struct vm_column* column);
int update_chunks() GNOMESCROLL_API;

}   // t_map
