/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace t_map
{

#if DC_SERVER
void teardown_day_cycle();
#endif

#if DC_CLIENT
void teardown_light_texture();
#endif

}   // t_map
