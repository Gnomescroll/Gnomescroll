#pragma once

#if GS_ASSIMP
# include <t_mob/assimp_includes.hpp>
# include <t_mob/common.hpp>
# include <t_mob/model_loader.cpp>
# include <t_mob/body_animation.cpp>
# include <t_mob/body_part_mesh.cpp>
# include <t_mob/body_mesh.cpp>
# include <t_mob/_interface.cpp>
#else
# include <t_mob/_interface.hpp>
#endif

