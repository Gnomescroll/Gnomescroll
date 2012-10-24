#pragma once

#include <t_map/common/constants.hpp>
#include <t_map/common/types.hpp>
#include <t_map/common/map_element.hpp>
#include <t_map/t_map_class.cpp>
#include <t_map/t_map.cpp>
#include <t_map/t_properties.cpp>
#include <t_map/net/t_CtoS.cpp>
#include <t_map/net/t_StoC.cpp>
#include <t_map/glsl/texture.cpp>
#include <t_map/chunk_special.cpp>
#include <t_map/map_special.cpp>
#include <t_map/lighting.hpp>
#include <t_map/config/_interface.hpp>
#include <t_map/config/block_dat.hpp>
#include <t_map/config/drop_dat.cpp>

#if DC_CLIENT
# include <t_map/config/textures.hpp>
# include <t_map/t_vbo_draw.cpp>
# include <t_map/t_vbo_update.cpp>
# include <t_map/glsl/cache.cpp>
# include <t_map/glsl/shader.cpp>
# include <t_map/glsl/palette.hpp>
# include <t_map/glsl/_interface.cpp>
# include <t_map/ortho_projection.cpp>
#endif

#if DC_SERVER
# include <t_map/server/manager.cpp>
# include <t_map/server/manager_class.hpp>
# include <t_map/server/subscription_list.hpp>
# include <t_map/server/env_process.cpp>
#endif

#include <t_map/_interface.cpp>
