#pragma once

#include <t_map/common/constants.hpp>

typedef enum
{
    CUBE_MATERIAL_NONE = 0,
    CUBE_MATERIAL_DIRT,
    CUBE_MATERIAL_STONE,
    CUBE_MATERIAL_DECORATION
} CubeMaterial;

typedef enum
{
    EMPTY_CUBE = 0,
    ERROR_CUBE = MAX_CUBES - 1,
    NULL_CUBE  = MAX_CUBES,
} CubeID;

typedef enum
{
    EmptyCube,
    ErrorCube,
    SolidCube,
    ItemContainerCube
} CubeType;

typedef enum
{
    CHUNK_HEIGHT_UNCHANGED = 0,
    CHUNK_HEIGHT_CHANGED = 1,
    CHUNK_HEIGHT_UNSET = 2
} ChunkHeightStatus;
