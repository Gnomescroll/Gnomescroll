#pragma once

namespace t_mob
{

class BodyAnimation
{
    /*
        Assume all keyframes contain the same nodes
    */
    public:
        char* animation_name;
        float length;   //length of animation

        struct _quanterion
        {
            float x,y,z,w;
        };

        //copy these pointers over
        char** nnl;                           //node name list
        struct Mat4* node_mTransformation;    //node bind transform list
        int nm;                               //node max

    BodyAnimation() :
        animation_name(NULL), length(0), nnl(NULL),
        node_mTransformation(NULL), nm(0)
    {}

    struct Mat4 get_anim_matrix_old(int frame_time, aiNodeAnim** node_channels,
                                    int node_channel_max, aiNode* node);
};

}   // t_mob
