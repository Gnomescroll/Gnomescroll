#pragma once

#if GS_ASSIMP
# include <t_mob/models/assimp_includes.hpp>
# include <t_mob/models/common.hpp>
# include <t_mob/models/model_loader.cpp>
# include <t_mob/models/body_animation.cpp>
# include <t_mob/models/body_part_mesh.cpp>
# include <t_mob/models/body_mesh.cpp>
# include <t_mob/models/_interface.cpp>
# include <t_mob/sprites/common.hpp>
# include <t_mob/sprites/_interface.cpp>
# include <t_mob/sprites/textures.cpp>
#else
# include <t_mob/_interface.hpp>
#endif

