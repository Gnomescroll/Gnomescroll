#pragma once

#ifdef __MSVC__
extern "C"
{
#endif

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h> //defines for postprocessor
#include <assimp/config.h>

#ifdef __MSVC__
}
#endif

#include <t_mob/common.hpp>
#include <t_mob/_interface.cpp>
#include <t_mob/model_loader.cpp>
#include <t_mob/body_animation.cpp>
#include <t_mob/body_part_mesh.cpp>
#include <t_mob/body_mesh.cpp>
