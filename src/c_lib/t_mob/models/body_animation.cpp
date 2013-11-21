/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "body_animation.hpp"

#include <t_mob/models/assimp_includes.hpp>

namespace t_mob
{

struct Mat4 BodyAnimation::get_anim_matrix_old(
    int frame_time, aiNodeAnim** node_channels, int node_channel_max, aiNode* node)
{
    for (int i=0; i<node_channel_max; i++)
    {
        aiNodeAnim* anim = node_channels[i];
        if (strcmp(anim->mNodeName.data, node->mName.data) == 0)
        {
            GS_ASSERT(anim->mNumPositionKeys == anim->mNumRotationKeys);
            int tmax = anim->mNumPositionKeys;
            aiVectorKey pos =  anim->mPositionKeys[frame_time % tmax];
            aiQuatKey rot = anim->mRotationKeys[frame_time % tmax];
            GS_ASSERT(pos.mTime == rot.mTime);
            //CONVERT TO MATRIX
            return quantenion_to_rotation_matrix(rot.mValue, pos.mValue);
        }
    }
    //if cannot find, then node does not have an animation and use this
    return _convert_matrix(node->mTransformation);
}

}   // t_mob
